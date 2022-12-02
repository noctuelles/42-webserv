import cgi, os
import cgitb; cgitb.enable()
form = cgi.FieldStorage()

fileitem = form['filename']

if fileitem.filename:

   fn = os.path.basename(fileitem.filename)
   open('/tmp/' + fn, 'wb').write(fileitem.file.read())
   message = 'The file "' + fn + '" was uploaded successfully. Thanks you for using this dump CGI script.'
 
else:
   message = 'No file was uploaded'
 
print ("""\
Content-Type: text/html

<html>
<body>
   <p>%s</p>
</body>
</html>
""" % (message,))
