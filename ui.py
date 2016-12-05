from Tkinter import *
#import serial

#Create & Configure root 
root = Tk()
Grid.rowconfigure(root, 0, weight=1)
Grid.columnconfigure(root, 0, weight=1)

#Create & Configure frame 
frame=Frame(root)
frame.grid(row=0, column=0, sticky=N+S+E+W)

matrix = [[0 for i in range(13)] for j in range(13)]

class MyButton:
    def __init__(self, x, y):
        self.btn =  Button(frame, text = "X", command=lambda row=x, column=y: self.toggle_text(row, column))

    def toggle_text(self, row, column):
	    """toggle button text between X and O"""
	    if self.btn["text"] == "X":
	        self.btn["text"] = "O"
	        matrix[row][column] = 1
	    else:
	        self.btn["text"] = "X"
	        matrix[row][column] = 0

class OKButton:
    def __init__(self):
        self.btn = Button(frame, text = "SEND",command=self.printMatrix)

    def printMatrix(self):
    	# TODO: SEND STUFF!!!
        print matrix
    	#ser = serial.Serial(1411,9600)
        #ser.write(matrix)

#Create a 13x13 (rows x columns) grid of buttons inside the frame
for row_index in range(13):
    Grid.rowconfigure(frame, row_index, weight=1)
    for col_index in range(13):
        myB = MyButton(row_index, col_index)
        myB.btn.grid(row=row_index, column=col_index, sticky=N+S+E+W)

okButt = OKButton()
okButt.btn.grid(row=6, column=14, sticky=N+S+E+W)

root.mainloop()