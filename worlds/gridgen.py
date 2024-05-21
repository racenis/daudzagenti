# generates sectors on a grid
# usage: py gridgen.py > world.lisp

len_x = 9
len_y = 9

for x in range(0, len_x):
	for y in range(0, len_y):
		w1 = "true" if y == 0 else "false"
		w2 = "true" if x == len_x - 1 else "false"
		w3 = "true" if y == len_y - 1 else "false"
		w4 = "true" if x == 0 else "false"

		print("(sector (")		
		print("\t(wall " + w1 + " " + w2 + " " + w3 + " " + w4 + " true true)")
		print("\t(x " + str(x) + ".0 " + str(x+1) + ".0 " + str(x+1) + ".0 " + str(x) + ".0)")
		print("\t(z " + str(y) + ".0 " + str(y) + ".0 " + str(y+1) + ".0 " + str(y+1) + ".0)")
		print("\t(y-max 5.0 5.0 5.0 5.0)")
		print("\t(y-min 0.0 0.0 0.0 0.0)")
		print("))\n")