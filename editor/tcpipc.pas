unit TCPIPC;

{$mode ObjFPC}{$H+}
{$assertions on}

interface

uses
  Classes, SysUtils, Dialogs, StrUtils, LazLogger, ssockets, fgl;

type
  QueryResponse = array of array of AnsiString;
  QueryDictionary  = specialize TFPGMap<AnsiString, AnsiString>;

// Public interface functions and stuff
function Connect(IP: string; Port: integer): boolean;
function Disconnect(): boolean;
function IsConnected(): boolean;
function Query(QueryString: string): QueryResponse;
procedure DisplayQuery(Query: QueryResponse);
function QueryResponseToDictionary(Query: QueryResponse): QueryDictionary;

// TODO: add a IsError(Query: QueryResponse): boolean; function
function GetError(Query: QueryResponse): AnsiString;

implementation

var
  IsSocketConnected: boolean;
  Socket: TInetSocket;

// Creates a connection to an instance of the simulation.
function Connect(IP: string; Port: integer): boolean;
var
  Startup: String[255];
begin
  Assert(not IsSocketConnected, 'Already connected to server!');

  try
   Socket := TInetSocket.Create(IP, Port, 1000);
  except
   on E: ESocketError do
      exit(false);
  end;

  Socket.Read(Startup[1], 255);

  IsSocketConnected := true;
  exit(true);
end;

// Disconnect the program from simulation.
function Disconnect(): boolean;
var
  ExitMessage: string;
begin
  Assert(IsSocketConnected, 'Not connected to server! Disconnect impossible!');
  ExitMessage := 'EXIT'#10;
  Socket.Write(ExitMessage[1], Length(ExitMessage));
  Socket.Free;
  IsSocketConnected := false;
  Result := true;
end;

// Returns true if is connected to simulation.
function IsConnected(): boolean;
begin
     Result := IsSocketConnected;
end;

// Performs a query and returns the result.
function Query(QueryString: string) : QueryResponse;
var
  Response: string[255];
  ResponseLen: integer;
  AllResponses: AnsiString;
  SplitIntoLines: array of AnsiString;
  LineElem: AnsiString;
  MergingLine: AnsiString;
  IsMerging: boolean;
  MergedLines: array of AnsiString;
  Line: integer;
  TrimLine: AnsiString;
begin
     Assert(IsSocketConnected, 'Need to Connect() to server before Query()!');

     Socket.Write(QueryString[1], Length(QueryString));

     AllResponses := '';

     repeat
       ResponseLen := Socket.Read(Response[1], 255);
       SetLength(Response, ResponseLen);
       AllResponses += Response;
     until String(Response).Contains('END');

     // TODO: investigate a better way to parse query responses
     SplitIntoLines := SplitString(Trim(AllResponses), #13#10);

     setLength(Result, Length(SplitIntoLines) - 1);

     for Line := 0 to Length(SplitIntoLines) - 2 do
     begin
          TrimLine := Trim(SplitIntoLines[Line]);

          // this bit here will merge strings back.
          // like a response 'hello "this is string"' will get broken up into
          // 'hello' '"this' 'is' 'string"' and we want to merge it back into
          // 'hello' 'this is string'
          MergedLines := [];
          IsMerging := false;

          for LineElem in SplitString(TrimLine, ' ') do
          begin

               if IsMerging then
                  if (RightStr(LineElem, 1) = '"') then
                  begin
                    MergingLine += ' ';
                    MergingLine += StringReplace(LineElem, '"', '', [rfReplaceAll]);

                    IsMerging := false;
                    MergedLines := Concat(MergedLines, [MergingLine]);
                  end
                  else
                  begin
                       MergingLine += ' ';
                       MergingLine += StringReplace(LineElem, '"', '', [rfReplaceAll]);
                  end

               else

               if (LeftStr(LineElem, 1) = '"') and (RightStr(LineElem, 1) = '"') then
                  MergedLines := Concat(MergedLines, [StringReplace(LineElem, '"', '', [rfReplaceAll])])
               else if (LeftStr(LineElem, 1) = '"') then
                  begin
                    MergingLine := StringReplace(LineElem, '"', '', [rfReplaceAll]);
                    IsMerging := true;
                  end
               else
                 MergedLines := Concat(MergedLines, [LineElem]);


          end;

          Result[Line] := MergedLines;
     end;
end;

// Prints a QueryResonse to debug console.
procedure DisplayQuery(Query: QueryResponse);
var
  output: AnsiString;
  line: array of AnsiString;
  col: AnsiString;
begin
  DebugLn('Query:');
  output := '';
  for line in Query do
  begin
    for col in line do
    begin
      output += col;
      output += ';';
    end;
    output += #10;
  end;
  //ShowMessage(output);
  DebugLn(output);
end;

// Converts a QueryResponse with 2 columns to a dictionary.
function QueryResponseToDictionary(Query: QueryResponse): QueryDictionary;
var
  line: array of AnsiString;
begin
  Result := QueryDictionary.Create;
  for line in Query do
  begin
    if line[0] = 'SKIP' then continue;
    Result.Add(line[1], line[2]);
  end;
end;

// Converts a QueryResponse containing an error to an error message string.
function GetError(Query: QueryResponse): AnsiString;
var
  token: AnsiString;
begin
  Result := '';
  for token in Query[0] do
  begin
    if token = 'ERROR' then continue;
    Result += token + ' ';
  end;
end;

initialization
begin
  IsSocketConnected := false;
end;

end.

