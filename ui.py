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
buttons = []

class MyButton:
    def __init__(self, x, y):
        self.btn =  Button(frame, text = "X", command=lambda row=x, column=y: self.toggle_text(row, column))
        self.xval = x
        self.yval = y

    def toggle_text(self, row, column):
        """toggle button text between R and G and B"""
        if self.btn["text"] == "X":
            self.btn["text"] = "R"
            self.btn["highlightbackground"] ="red"
            matrix[row][column] = 3
        elif self.btn["text"] == "R":
            self.btn["text"] = "O"
            self.btn["highlightbackground"] ="orange"
            matrix[row][column] = 6
        elif self.btn["text"] == "O":
            self.btn["text"] = "Y"
            self.btn["highlightbackground"] ="yellow"
            matrix[row][column] = 4
        elif self.btn["text"] == "Y":
            self.btn["text"] = "G"
            self.btn["highlightbackground"] ="green"
            matrix[row][column] = 1
        elif self.btn["text"] == "G":
            self.btn["text"] = "B"
            self.btn["highlightbackground"] ="blue"
            matrix[row][column] = 2
        elif self.btn["text"] == "B":
            self.btn["text"] = "P"
            self.btn["highlightbackground"] ="purple"
            matrix[row][column] = 5
        elif self.btn["text"] == "P":
            self.btn["text"] = "X"
            self.btn["highlightbackground"] ="white"
            matrix[row][column] = 0

def sendInfo():
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

def resetButtons():
    for i in range(13):
        for j in range(13):
            if matrix[i][j] == 0:
                buttons[i*13+j].btn["text"] = "X"
                buttons[i*13+j].btn["highlightbackground"] ="white"
            elif matrix[i][j] == 1:
                buttons[i*13+j].btn["text"] = "G"
                buttons[i*13+j].btn["highlightbackground"] ="green"
            elif matrix[i][j] == 2:
                buttons[i*13+j].btn["text"] = "B"
                buttons[i*13+j].btn["highlightbackground"] ="blue"
            elif matrix[i][j] == 3:
                buttons[i*13+j].btn["text"] = "R"
                buttons[i*13+j].btn["highlightbackground"] ="red"
            elif matrix[i][j] == 4:
                buttons[i*13+j].btn["text"] = "Y"
                buttons[i*13+j].btn["highlightbackground"] ="yellow"
            elif matrix[i][j] == 5:
                buttons[i*13+j].btn["text"] = "P"
                buttons[i*13+j].btn["highlightbackground"] ="purple"
            elif matrix[i][j] == 6:
                buttons[i*13+j].btn["text"] = "O"
                buttons[i*13+j].btn["highlightbackground"] ="orange"

class sendButton:
    def __init__(self):
        self.btn = Button(frame, text = "SEND",command=sendInfo)

class clearButton:
    def __init__(self):
        self.btn = Button(frame, text = "CLEAR",command=self.clear)

    def clear(self):
        # reset matrix and buttons
        for i in range(13):
            for j in range(13):
                matrix[i][j] = 0
        for button in buttons:
            button.btn["text"] = "X"
            button.btn["highlightbackground"] ="white"
        sendInfo() 

class rainbowButton:
    def __init__(self):
        self.btn = Button(frame, text = "RAINBOW",command=self.sendRainbow)

    def sendRainbow(self):
        # reset buttons and matrix
        counter = 0
        for i in range(13):
            for j in range(13):
                matrix[i][j] = (counter % 6) + 1
                counter = counter + 1
        resetButtons()
        sendInfo()

class surpriseButton:
    def __init__(self):
        self.btn = Button(frame, text = "SURPRISE",command=self.sendSurprise)

    def sendSurprise(self):
        # reset buttons and matrix
        matrix = []
        sendInfo()

#Create a 13x13 (rows x columns) grid of buttons inside the frame
for row_index in range(13):
    Grid.rowconfigure(frame, row_index, weight=1)
    for col_index in range(13):
        myB = MyButton(row_index, col_index)
        myB.btn.grid(row=row_index, column=col_index, sticky=N+S+E+W)
        buttons.append(myB)

sendButt = sendButton()
sendButt.btn.grid(row=4, column=14, sticky=N+S+E+W)
clearButt = clearButton()
clearButt.btn.grid(row=5, column=14, sticky=N+S+E+W)

rainbowButt = rainbowButton()
rainbowButt.btn.grid(row=7, column=14, sticky=N+S+E+W)
surpriseButt = surpriseButton()
surpriseButt.btn.grid(row=8, column=14, sticky=N+S+E+W)


root.mainloop()
