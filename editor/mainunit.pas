unit MainUnit;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, ExtCtrls, ComCtrls,
  Menus, Grids, ValEdit, StdCtrls, TAGraph, TASeries, LazLogger, TCPIPC;

type

  { TMainForm }

  TMainForm = class(TForm)
    ChemicalSeriesA: TLineSeries;
    ChemicalSeriesB: TLineSeries;
    ChemicalSeriesC: TLineSeries;
    ChemicalSeriesD: TLineSeries;
    ChemicalSeriesE: TLineSeries;
    ChemicalSeriesF: TLineSeries;
    ChemicalSeriesG: TLineSeries;
    ChemicalSeriesH: TLineSeries;
    ChemicalComboE: TComboBox;
    ChemicalComboF: TComboBox;
    ChemicalComboG: TComboBox;
    ChemicalComboH: TComboBox;
    ChemicalComboA: TComboBox;
    ChemicalComboB: TComboBox;
    ChemicalComboC: TComboBox;
    ChemicalComboD: TComboBox;
    SectorList: TStringGrid;
    SectorRightPanel: TPanel;
    KillButton: TButton;
    InjectAtX: TEdit;
    InjectAtY: TEdit;
    InjectAtZ: TEdit;
    InjectPrototypeAt: TButton;
    InjectPrototype: TButton;
    Label1: TLabel;
    RefreshSectorList: TButton;
    RefreshPrototypeList: TButton;
    ChemicalTrackerCheckbox: TCheckBox;
    ChemicalChart: TChart;
    RefreshChemicals: TButton;
    ComboChemical: TComboBox;
    ChemicalPanelBottom: TPanel;
    ChemicalPanelTop: TPanel;
    RadioChemicalsSectors: TRadioButton;
    RadioChemicalsAgents: TRadioButton;
    RefreshAgentList: TButton;
    MainMenu: TMainMenu;
    FileMenuItem: TMenuItem;
    DisconnectMenuItem: TMenuItem;
    ConnectMenuItem: TMenuItem;
    PageControl1: TPageControl;
    AgentsRightPanel: TPanel;
    PrototypesRightPanel: TPanel;
    QuitMenuItem: TMenuItem;
    Separator1: TMenuItem;
    StatusBar: TStatusBar;
    AgentTab: TTabSheet;
    PrototypeTab: TTabSheet;
    SectorTab: TTabSheet;
    ChemicalTab: TTabSheet;
    AgentList: TStringGrid;
    PrototypeList: TStringGrid;
    AgentProperties: TValueListEditor;
    ChemicalTimer: TTimer;
    VariableTab: TTabSheet;
    NeuronTab: TTabSheet;
    procedure AgentListSelection(Sender: TObject; aCol, aRow: Integer);
    procedure AgentPropertiesSetEditText(Sender: TObject; ACol, ARow: Integer;
      const Value: string);
    procedure KillButtonClick(Sender: TObject);
    procedure ChemicalComboAChange(Sender: TObject);
    procedure ChemicalComboBChange(Sender: TObject);
    procedure ChemicalComboCChange(Sender: TObject);
    procedure ChemicalComboDChange(Sender: TObject);
    procedure ChemicalComboEChange(Sender: TObject);
    procedure ChemicalComboFChange(Sender: TObject);
    procedure ChemicalComboGChange(Sender: TObject);
    procedure ChemicalComboHChange(Sender: TObject);
    procedure ChemicalTimerTimer(Sender: TObject);
    procedure ChemicalTrackerCheckboxChange(Sender: TObject);
    procedure ComboChemicalChange(Sender: TObject);
    procedure ConnectMenuItemClick(Sender: TObject);
    procedure DisconnectMenuItemClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var CloseAction: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure InjectPrototypeAtClick(Sender: TObject);
    procedure InjectPrototypeClick(Sender: TObject);
    procedure PrototypeListSelection(Sender: TObject; aCol, aRow: Integer);
    procedure QuitMenuItemClick(Sender: TObject);
    procedure RadioChemicalsAgentsChange(Sender: TObject);
    procedure RadioChemicalsSectorsChange(Sender: TObject);
    procedure RefreshAgentListClick(Sender: TObject);
    procedure RefreshChemicalsClick(Sender: TObject);
    procedure RefreshPrototypeListClick(Sender: TObject);
    procedure RefreshSectorListClick(Sender: TObject);
    procedure SectorListSelection(Sender: TObject; aCol, aRow: Integer);
  private

  public

  end;

var
  MainForm: TMainForm;

implementation

type
  SelectionType = (None, Agent, Sector, Prototype);

var
  SelectedAgent: Integer; // this is the agent that is displayed in right pane
  SelectedSector: Integer;
  SelectedPrototype: String;
  LastSelected: SelectionType;

  Chemical : String;
  Chemicals: array of String;

{$R *.lfm}

{ TMainForm }

// ************************************************************************** //
// *                             FILE MENU                                  * //
// ************************************************************************** //

// File > Quit
procedure TMainForm.QuitMenuItemClick(Sender: TObject);
begin
  Close;
end;

// File > Connect
procedure TMainForm.ConnectMenuItemClick(Sender: TObject);
var
  Response: QueryResponse;
  Chemical: AnsiString;
  ChemicalRecord: array of AnsiString;
begin
  StatusBar.SimpleText := 'Connecting...';
  StatusBar.Refresh;

  // In the future it might be a good idea to create a special dialog, which
  // allows the user to input the IP address and the port, but until the
  // simulation allows connection from remote clients and/or changing the
  // default port, this should work fine.
  if Connect('127.0.0.1', 4444) then
     begin
          RefreshAgentList.Enabled := true;
          RefreshPrototypeList.Enabled := true;
          RefreshSectorList.Enabled := true;

          ConnectMenuItem.Enabled := false;
          DisconnectMenuItem.Enabled := true;

          // retrieve the list of all chemicals that are registered into
          // the simulation
          Response := Query('GET CHEMICALS');

          Chemicals := [];

          for ChemicalRecord in Response do
              begin
                Chemicals := Concat(Chemicals, [ChemicalRecord[1]]);
              end;

          ChemicalComboA.Items.Clear;
          ChemicalComboB.Items.Clear;
          ChemicalComboC.Items.Clear;
          ChemicalComboD.Items.Clear;
          ChemicalComboE.Items.Clear;
          ChemicalComboF.Items.Clear;
          ChemicalComboG.Items.Clear;
          ChemicalComboH.Items.Clear;

          for Chemical in Chemicals do
               begin

                 ChemicalComboA.Items.Add(Chemical);
                 ChemicalComboB.Items.Add(Chemical);
                 ChemicalComboC.Items.Add(Chemical);
                 ChemicalComboD.Items.Add(Chemical);
                 ChemicalComboE.Items.Add(Chemical);
                 ChemicalComboF.Items.Add(Chemical);
                 ChemicalComboG.Items.Add(Chemical);
                 ChemicalComboH.Items.Add(Chemical);

               end;

     end
  else
      ShowMessage('Start the simulation before connecting!');
  StatusBar.SimpleText := 'Ready.';
end;

// File > Disconnect
procedure TMainForm.DisconnectMenuItemClick(Sender: TObject);
begin
  Disconnect;

  RefreshAgentList.Enabled := false;
  RefreshPrototypeList.Enabled := false;
  RefreshSectorList.Enabled := false;

  ConnectMenuItem.Enabled := true;
  DisconnectMenuItem.Enabled := false;
end;

// ************************************************************************** //
// *                              AGENT TAB                                 * //
// ************************************************************************** //

// Agent Tab > Bottom Panel > Refresh
procedure TMainForm.RefreshAgentListClick(Sender: TObject);
var
  Agents: QueryResponse;
  X: Integer;
  Y: Integer;
begin
  StatusBar.SimpleText := 'Refreshing...';
  StatusBar.Refresh;
  AgentsRightPanel.Enabled := false;

  Agents := Query('GET AGENTS');

  AgentList.RowCount := Length(Agents) + 1; // plus header row

  for X := 0 to 3 do
      for Y:= 0 to Length(Agents) - 1 do
          AgentList.Cells[X, Y+1] := Agents[Y, X+1];

  AgentList.Enabled := true;
  ComboChemical.Enabled := true;
  RadioChemicalsAgents.Enabled := true;

  StatusBar.SimpleText := 'Ready.';
end;

// Agent Tab > Agent List
procedure TMainForm.AgentListSelection(Sender: TObject; aCol, aRow: Integer);
var
  Response: QueryResponse;
  Properties: QueryDictionary;
begin
  StatusBar.SimpleText := 'Querying agent...';
  StatusBar.Refresh;
  AgentsRightPanel.Enabled := false;

  Response := Query('GET AGENT ' + AgentList.Cells[0, aRow]);

  if Response[0, 0] = 'ERROR' then
     begin
       // for some reason showing a message box here enters a loop??
       StatusBar.SimpleText := 'Error: ' + GetError(Response);
       exit;
     end;

  // might be a good idea to put this into a function
  SelectedAgent := StrToInt(AgentList.Cells[0, aRow]);
  LastSelected := Agent;
  RadioChemicalsAgents.Checked := true;
  RadioChemicalsSectors.Checked := false;
  ChemicalPanelBottom.Enabled := true;

  Properties := QueryResponseToDictionary(Response);

  AgentProperties.Clear;
  AgentProperties.InsertRow('id', Properties['id'], true);
  AgentProperties.InsertRow('name', Properties['name'], true);
  AgentProperties.InsertRow('category-1', Properties['category-1'], true);
  AgentProperties.InsertRow('category-2', Properties['category-2'], true);
  AgentProperties.InsertRow('category-3', Properties['category-3'], true);
  AgentProperties.InsertRow('position-x', Properties['position-x'], true);
  AgentProperties.InsertRow('position-y', Properties['position-y'], true);
  AgentProperties.InsertRow('position-z', Properties['position-z'], true);
  AgentProperties.InsertRow('move-speed', Properties['move-speed'], true);
  AgentProperties.InsertRow('turn-speed', Properties['turn-speed'], true);
  AgentProperties.InsertRow('accl-speed', Properties['accl-speed'], true);
  AgentProperties.InsertRow('rotation', Properties['rotation'], true);
  AgentProperties.InsertRow('mass', Properties['mass'], true);


  AgentProperties.ItemProps['id'].ReadOnly := true;
  AgentProperties.ItemProps['name'].ReadOnly := true;

  AgentsRightPanel.Enabled := true;
  StatusBar.SimpleText := 'Ready.';
end;

// Agent Tab > Right Panel > Agent Property List
procedure TMainForm.AgentPropertiesSetEditText(Sender: TObject; ACol,
  ARow: Integer; const Value: string);
var
  Response: QueryResponse;
begin
  StatusBar.SimpleText := 'Updating agent...';
  StatusBar.Refresh;

  Response := Query('SET AGENT ' + IntToStr(SelectedAgent) + ' ' + AgentProperties.Keys[ARow] + ' ' + Value);

  if Response[0, 0] = 'ERROR' then
     StatusBar.SimpleText := 'Error: ' + GetError(Response)
  else
  StatusBar.SimpleText := 'OK';
end;

// Agent Tab > Right Panel > Kill
procedure TMainForm.KillButtonClick(Sender: TObject);
var
  Response: QueryResponse;
begin
  StatusBar.SimpleText := 'Killing agent...';
  StatusBar.Refresh;

  Response := Query('KILL ' + IntToStr(SelectedAgent));

  if Response[0, 0] = 'ERROR' then
     StatusBar.SimpleText := 'Error: ' + GetError(Response)
  else
  StatusBar.SimpleText := 'OK';

end;

// ************************************************************************** //
// *                             PROTOTYPE TAB                              * //
// ************************************************************************** //

// Prototype Tab > Bottom Panel > Refresh
procedure TMainForm.RefreshPrototypeListClick(Sender: TObject);
var
  Prototypes: QueryResponse;
  X: Integer;
  Y: Integer;
begin
  StatusBar.SimpleText := 'Refreshing...';
  StatusBar.Refresh;
  PrototypesRightPanel.Enabled := false;

  Prototypes := Query('GET PROTOTYPES');

  DisplayQuery(Prototypes);

  PrototypeList.RowCount := Length(Prototypes) + 1; // plus header row

  for X := 0 to 2 do
      for Y:= 0 to Length(Prototypes) - 1 do
          PrototypeList.Cells[X, Y+1] := Prototypes[Y, X+1];

  PrototypeList.Enabled := true;
  StatusBar.SimpleText := 'Ready.';
end;

// Prototype Tab > Prototype List
procedure TMainForm.PrototypeListSelection(Sender: TObject; aCol, aRow: Integer
  );
var
  Response: QueryResponse;
  Properties: QueryDictionary;
begin
  StatusBar.SimpleText := 'Querying prototype...';
  StatusBar.Refresh;
  PrototypesRightPanel.Enabled := false;

  // since this query is not implemented into the simulation yet, I will
  // comment it out for now
  (*Response := Query('GET PROTOTYPE ' + PrototypeList.Cells[0, aRow]);

  if Response[0, 0] = 'ERROR' then
     begin
       // for some reason showing a message box here enters a loop??
       StatusBar.SimpleText := 'Error: ' + GetError(Response);
       exit;
     end;*)

  SelectedPrototype := PrototypeList.Cells[0, aRow];

  (*Properties := QueryResponseToDictionary(Response);
  InjectAtX.Text := Properties['position-x'];
  InjectAtY.Text := Properties['position-y'];
  InjectAtZ.Text := Properties['position-z'];*)

  PrototypesRightPanel.Enabled := true;
  StatusBar.SimpleText := 'Ready.';
end;

// Prototype Tab > Right Panel > Inject
procedure TMainForm.InjectPrototypeClick(Sender: TObject);
var
  Response: QueryResponse;
begin
  StatusBar.SimpleText := 'Injecting agent...';
  StatusBar.Refresh;

  Response := Query('INSTANTIATE ANY ' + SelectedPrototype);

  if Response[0, 0] = 'ERROR' then
     StatusBar.SimpleText := 'Error: ' + GetError(Response)
  else
      StatusBar.SimpleText := 'OK';
end;

// Prototype Tab > Right Panel > Inject At
procedure TMainForm.InjectPrototypeAtClick(Sender: TObject);
var
  Response: QueryResponse;
begin
  StatusBar.SimpleText := 'Injecting agent...';
  StatusBar.Refresh;

  Response := Query('INSTANTIATE AT '
           + SelectedPrototype + ' '
           + InjectAtX.Text + ' '
           + InjectAtY.Text + ' '
           + InjectAtZ.Text);

  if Response[0, 0] = 'ERROR' then
     StatusBar.SimpleText := 'Error: ' + GetError(Response)
  else
      StatusBar.SimpleText := 'OK';
end;

// ************************************************************************** //
// *                               SECTOR TAB                               * //
// ************************************************************************** //

// Sector Tab > Bottom Panel > Refresh
procedure TMainForm.RefreshSectorListClick(Sender: TObject);
var
  Sectors: QueryResponse;
  X: Integer;
  Y: Integer;
begin
  StatusBar.SimpleText := 'Refreshing...';
  StatusBar.Refresh;
  SectorRightPanel.Enabled := false;

  Sectors := Query('GET SECTORS');

  SectorList.RowCount := Length(Sectors) + 1; // plus header row

  for X := 0 to 2 do
      for Y:= 0 to Length(Sectors) - 1 do
          SectorList.Cells[X, Y+1] := Sectors[Y, X+1];

  SectorList.Enabled := true;
  ComboChemical.Enabled := true;
  RadioChemicalsSectors.Enabled := true;
  StatusBar.SimpleText := 'Ready.';
end;

// Sector Tab > Sector List
procedure TMainForm.SectorListSelection(Sender: TObject; aCol, aRow: Integer);
var
  Response: QueryResponse;
  Properties: QueryDictionary;
begin
  StatusBar.SimpleText := 'Querying sector...';
  StatusBar.Refresh;
  SectorRightPanel.Enabled := false;

  // since this query is not implemented yet, I will comment it out
  (*Response := Query('GET SECTOR ' + PrototypeList.Cells[0, aRow]);

  if Response[0, 0] = 'ERROR' then
     begin
       // for some reason showing a message box here enters a loop??
       StatusBar.SimpleText := 'Error: ' + GetError(Response);
       exit;
     end;*)

  SelectedSector := StrToInt(SectorList.Cells[0, aRow]);
  LastSelected := Sector;
  RadioChemicalsAgents.Checked := false;
  RadioChemicalsSectors.Checked := true;
  ChemicalPanelBottom.Enabled := true;

  (*Properties := QueryResponseToDictionary(Response);*)

  SectorRightPanel.Enabled := true;
  StatusBar.SimpleText := 'Ready.';
end;

// ************************************************************************** //
// *                             CHEMICAL TAB                               * //
// ************************************************************************** //

// Chemical Tab > Top Panel > Agents
procedure TMainForm.RadioChemicalsAgentsChange(Sender: TObject);
var
  row: Integer;
begin
  if not RadioChemicalsAgents.Checked then exit;

  // put the list of agents into combo box
  ComboChemical.Items.Clear;
  for row := 1 to AgentList.RowCount-1 do
      ComboChemical.Items.Add(AgentList.Cells[0, row] + ' : ' + AgentList.Cells[1, row]);
end;

// Chemical Tab > Top Panel > Sectors
procedure TMainForm.RadioChemicalsSectorsChange(Sender: TObject);
var
  row: Integer;
begin
  if not RadioChemicalsSectors.Checked then exit;

  // put the list of sectors into combo box
  ComboChemical.Items.Clear;
  for row := 1 to SectorList.RowCount-1 do
      ComboChemical.Items.Add(SectorList.Cells[0, row] + ' : ' + SectorList.Cells[1, row]);
end;

// Chemical Tab > Top Panel > Combo Box
procedure TMainForm.ComboChemicalChange(Sender: TObject);
var
  SelectedThingID: AnsiString;
begin
  if RadioChemicalsAgents.Checked then
     begin
        SelectedThingID := AgentList.Cells[0, ComboChemical.ItemIndex+1];
        SelectedAgent := StrToInt(SelectedThingID);
        LastSelected := Agent;
        ChemicalPanelBottom.Enabled := true;
     end;

  if RadioChemicalsSectors.Checked then
     begin
        SelectedThingID := SectorList.Cells[0, ComboChemical.ItemIndex+1];
        SelectedSector := StrToInt(SelectedThingID);
        LastSelected := Sector;
        ChemicalPanelBottom.Enabled := true;
     end;

end;

// Chemical Tab > Bottom Panel > Refresh
procedure TMainForm.RefreshChemicalsClick(Sender: TObject);
var
  Response: QueryResponse;
  Chemicals: QueryDictionary;
begin
  if LastSelected = Sector then
     Response := Query('GET CHEMICAL SECTOR ' + IntToStr(SelectedSector))
  else
      Response := Query('GET CHEMICAL AGENT ' + IntToStr(SelectedAgent));

  if Response[0, 0] = 'ERROR' then
     begin
       StatusBar.SimpleText := 'Error: ' + GetError(Response);
       exit;
     end;

  Chemicals := QueryResponseToDictionary(Response);

  // if an agent has zero level of a chemical, then it won't be transmitted
  // through the IPC API, so we need to insert a default value
  if Chemicals.IndexOf(ChemicalSeriesA.Title) = -1 then Chemicals.Add(ChemicalSeriesA.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesB.Title) = -1 then Chemicals.Add(ChemicalSeriesB.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesC.Title) = -1 then Chemicals.Add(ChemicalSeriesC.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesD.Title) = -1 then Chemicals.Add(ChemicalSeriesD.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesE.Title) = -1 then Chemicals.Add(ChemicalSeriesE.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesF.Title) = -1 then Chemicals.Add(ChemicalSeriesF.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesG.Title) = -1 then Chemicals.Add(ChemicalSeriesG.Title, '0');
  if Chemicals.IndexOf(ChemicalSeriesH.Title) = -1 then Chemicals.Add(ChemicalSeriesH.Title, '0');

  ChemicalSeriesA.AddY(StrToInt(Chemicals[ChemicalSeriesA.Title]));
  ChemicalSeriesB.AddY(StrToInt(Chemicals[ChemicalSeriesB.Title]));
  ChemicalSeriesC.AddY(StrToInt(Chemicals[ChemicalSeriesC.Title]));
  ChemicalSeriesD.AddY(StrToInt(Chemicals[ChemicalSeriesD.Title]));
  ChemicalSeriesE.AddY(StrToInt(Chemicals[ChemicalSeriesE.Title]));
  ChemicalSeriesF.AddY(StrToInt(Chemicals[ChemicalSeriesF.Title]));
  ChemicalSeriesG.AddY(StrToInt(Chemicals[ChemicalSeriesG.Title]));
  ChemicalSeriesH.AddY(StrToInt(Chemicals[ChemicalSeriesH.Title]));
end;


// Chemical Tab > Combo Boxes
procedure TMainForm.ChemicalComboAChange(Sender: TObject);
begin
  if ChemicalComboA.ItemIndex = -1 then exit;
  ChemicalSeriesA.Title := ChemicalComboA.Items[ChemicalComboA.ItemIndex];
end;

procedure TMainForm.ChemicalComboBChange(Sender: TObject);
begin
  if ChemicalComboB.ItemIndex = -1 then exit;
  ChemicalSeriesB.Title := ChemicalComboB.Items[ChemicalComboB.ItemIndex];
end;

procedure TMainForm.ChemicalComboCChange(Sender: TObject);
begin
  if ChemicalComboC.ItemIndex = -1 then exit;
  ChemicalSeriesC.Title := ChemicalComboC.Items[ChemicalComboC.ItemIndex];
end;

procedure TMainForm.ChemicalComboDChange(Sender: TObject);
begin
  if ChemicalComboD.ItemIndex = -1 then exit;
  ChemicalSeriesD.Title := ChemicalComboD.Items[ChemicalComboD.ItemIndex];
end;

procedure TMainForm.ChemicalComboEChange(Sender: TObject);
begin
  if ChemicalComboE.ItemIndex = -1 then exit;
  ChemicalSeriesE.Title := ChemicalComboE.Items[ChemicalComboE.ItemIndex];
end;

procedure TMainForm.ChemicalComboFChange(Sender: TObject);
begin
  if ChemicalComboF.ItemIndex = -1 then exit;
  ChemicalSeriesF.Title := ChemicalComboF.Items[ChemicalComboF.ItemIndex];
end;

procedure TMainForm.ChemicalComboGChange(Sender: TObject);
begin
  if ChemicalComboG.ItemIndex = -1 then exit;
  ChemicalSeriesG.Title := ChemicalComboG.Items[ChemicalComboG.ItemIndex];
end;

procedure TMainForm.ChemicalComboHChange(Sender: TObject);
begin
  if ChemicalComboH.ItemIndex = -1 then exit;
  ChemicalSeriesH.Title := ChemicalComboH.Items[ChemicalComboG.ItemIndex];
end;

procedure TMainForm.ChemicalTimerTimer(Sender: TObject);
begin
  DebugLn('Tick');
  RefreshChemicals.Click;
end;

procedure TMainForm.ChemicalTrackerCheckboxChange(Sender: TObject);
begin
  ChemicalTimer.Enabled := ChemicalTrackerCheckbox.Checked;
end;

// ************************************************************************** //
// *                              MAIN FORM                                 * //
// ************************************************************************** //

procedure TMainForm.FormCreate(Sender: TObject);
begin

end;

procedure TMainForm.FormClose(Sender: TObject; var CloseAction: TCloseAction);
begin
     if IsConnected then
        Disconnect;
end;

initialization
begin
  SelectedAgent := 0;
  SelectedSector := 0;
  SelectedPrototype := 'none';
  LastSelected := None;
end;

end.

