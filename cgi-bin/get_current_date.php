<html>
<?php
exec ( "/bin/date", $INFO, $return );  // execute the date command.
$DATE_INFO = $INFO[0];                 // copy the first line from the
                                       // returned array to $DATE_INFO
?>

<title>Current date</title>
<body>
<?php echo "The current date is $DATE_INFO";  ?>
</body>
</html>
