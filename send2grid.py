#!/usr/bin/python

f = open('input_sendgrid.txt', 'r')
destination = f.readline()
subject = f.readline()
source = f.readline()

lines = f.readlines()
html = ""
for line in lines:
	html = html + line + "<br>"

import sendgrid
# Authentication Information
sg = sendgrid.SendGridClient('shong010', 'gunbound1')

message = sendgrid.Mail()
message.add_to(destination)
message.set_subject(subject)
#message.set_text(text)
message.set_html(html)
message.set_from(source)

receive = sg.send(message)
