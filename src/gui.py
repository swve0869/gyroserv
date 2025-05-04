from tkinter import *
from tkinter import ttk
import tkinter as tk
import socket

UDP_IP = "10.160.0.182"
UDP_PORT = 8000
IP_ARDUINO = "10.160.0.86"
PORT_ARDUINO = 7000
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))


""" def send_net(kp,ki,kd):
   data, addr = sock.recvfrom(1024)
   #print(f"Received packet from {addr}: {data.decode('utf-8')}")
   message = "weights:{kp:.4f}:{ki:.4f}:{kd:.4f}"
   #message = message.format(kp.get(),ki.get(),kd.get())
   sock.sendto(message.encode(), (IP_ARDUINO, PORT_ARDUINO))
   root.after(10, send_net)  """

root = Tk()
root.geometry("500x500")  
frm = ttk.Frame(root, padding=10)
frm.grid()

kp = DoubleVar()
kd = DoubleVar()
ki = DoubleVar()
T  = IntVar()

f = open("default_pid_gains","r")
vals = f.readline().split(":")
kp.set(vals[0])
ki.set(vals[1])
kd.set(vals[2])
T.set (vals[3])
f.close() 


def scale_change(value):
   """  global kp
   global ki 
   global kd """
   p = kp.get()
   i = ki.get()
   d = kd.get()
   t = T.get()
   message = "{:.4f}:{:.4f}:{:.4f}:{:}"
   message = message.format(p,i,d,t)
   sock.sendto(message.encode(), (IP_ARDUINO, PORT_ARDUINO))
   #root.after(10, send_net) 



# kp slider

kp_scale = tk.Scale(frm,variable = kp,
                    from_ = 0.0, 
                    to = 4.0,
                    command = scale_change,
                    length = 400,
                    resolution = .01,
                    orient=tk.HORIZONTAL).grid(column=0, row=1)
kp_label = Label(frm, text = "Kp scaler").grid(column=0, row=2)

# ki slider
kp_scale = tk.Scale(frm,variable = ki,
                    from_ = 0.0, 
                    to = 0.01,
                    command = scale_change,
                    length = 400,
                    resolution = .0001,
                    orient=tk.HORIZONTAL).grid(column=0, row=3)
kd_label = Label(frm, text = "Ki scaler").grid(column=0, row=4)

# kd slider

kp_scale = tk.Scale(frm,variable = kd,
                    from_ = 0.0, 
                    to = 100.0,
                    command = scale_change,
                    length = 400,
                    resolution = .01,
                    orient=tk.HORIZONTAL).grid(column=0, row=5)
kd_label = Label(frm, text = "Kd scaler").grid(column=0, row=6)

t_scale = tk.Scale(frm,variable = T,
                    from_ = 0, 
                    to = 100,
                    command = scale_change,
                    length = 400,
                    resolution = 1,
                    orient=tk.HORIZONTAL).grid(column=0, row=7)
t_label = Label(frm, text = "Kd scaler").grid(column=0, row=8)


def save_gains():
    f = open("default_pid_gains","w")
    p = kp.get()
    i = ki.get()
    d = kd.get()
    t = T.get()
    message = "{:.4f}:{:.4f}:{:.4f}:{:}"
    message = message.format(p,i,d,t)
    f.write(message)
    f.close() 
set_gains_button = tk.Button(
                   text="Save PID gains", 
                   command=save_gains).grid(column=0, row=9)

#root.after(10, send_net)

root.mainloop()