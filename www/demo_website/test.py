# Import modules for CGI handling 
import cgi, cgitb 
import os



# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')


while (1):
    pass

print ("Content-type:text/html\r\n\r\n")
print ("<font size=+1>Environment</font></br>");
for param in os.environ.keys():
   print ("<b>%20s</b>: %s</br>" % (param, os.environ[param]))
print ("<html>")
print ("<head>")
print ("<title>Hello - Second CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s %s</h2>" % (first_name, last_name))
print ("</body>")
print ("</html>")
