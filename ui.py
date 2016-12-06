from Tkinter import *
import serial

serial_speed = 115200
serial_port = '/dev/cu.usbmodem1411'

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
        self.btn =  Button(frame, text = "R", command=lambda row=x, column=y: self.toggle_text(row, column))

    def toggle_text(self, row, column):
        """toggle button text between R and G and B"""
        if self.btn["text"] == "R":
            self.btn["text"] = "G"
            matrix[row][column] = 1
        elif self.btn["text"] == "G":
            self.btn["text"] = "B"
            matrix[row][column] = 2
        elif self.btn["text"] == "B":
            self.btn["text"] = "R"
            matrix[row][column] = 0
 
class OKButton:
    def __init__(self):
        self.btn = Button(frame, text = "SEND",command=self.printMatrix)

    def printMatrix(self):
    	# TODO: SEND STUFF!!!
        print matrix
        ser = serial.Serial(serial_port, serial_speed, timeout=1)
        string = ''
        new_strings = []
        for i in range(13):
            for j in range(13):
                string += str(matrix[i][j])
        for i in range(13):
            if i%2 == 0:
                new_strings.append(string[13*i: 13*i+13])
            else:
                new_strings.append(string[13*i+12:13*i-1:-1])
        string = "".join(new_strings)
        ser.write(string)
        print string
        ser.close()

#Create a 13x13 (rows x columns) grid of buttons inside the frame
for row_index in range(13):
    Grid.rowconfigure(frame, row_index, weight=1)
    for col_index in range(13):
        myB = MyButton(row_index, col_index)
        myB.btn.grid(row=row_index, column=col_index, sticky=N+S+E+W)

okButt = OKButton()
okButt.btn.grid(row=6, column=14, sticky=N+S+E+W)

root.mainloop()