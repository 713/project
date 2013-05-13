<?php
//=============================================================================
// Save user's email to file (user_email.txt) first. Then, upload user's data.
//=============================================================================

// Show webpage
echo <<<EOT
<!DOCTYPE html>
<html>

<head>
  <meta charset="utf-8" />
  <title>Team B's 03-713 project</title>
  <meta name="description" content="713 bioinformatics data integration" />
  <meta name="author" content="Luigi, Prateek, Rebecca, Stuti, Yiming" />
	<link rel="shortcut icon" type="image/ico" href="img/favicon.ico" />	
  <link href='http://fonts.googleapis.com/css?family=Fredoka+One' rel='stylesheet' type='text/css'>
  <link href="css/bootstrap.css" rel="stylesheet" media="screen" type="text/css" />
  <link href="css/style.css" rel="stylesheet" media="screen" type="text/css" />
</head>

<body>
  <br>
  <div class="container">
    <div class="hero-unit">
      <h1 style="color: #468847">Team B's 03-713 project</h1>
      <p class="lead">03-713: Bioinformatics Data Integration Practicum</p><br>
      <!-- class="warning" is yellow color; using as "pending" -->
      <div class="alert alert-warning">
        <h1>Uploading...</h1><br>
        <h3>Please wait until a green-colored<br>"You may close this window and wait
            for email." <br> message appears.</h3>
      </div>
      
      <!-- Placeholders for Upload Status; uses innerHTML -->
      <output id="uploadStatus"></output>

      <hr>
      <p><a href="https://github.com/713/project/">Link to our backends on Github</a>
        <small class="text-warning pull-right">( Team B members: Stuti A.,
          Rebecca E., Luigi L., Prateek T., Yiming X. )</small>
      </p>
    </div> <!-- hero-unit -->
  </div> <!-- container -->
  <script src="http://code.jquery.com/jquery-latest.js"></script>
  <script src="js/bootstrap.min.js"></script>
  
</body>
</html>

EOT;

//=============================================================================
// Saves user's email
//=============================================================================
$filename = "user_email.txt"; // Requires write permission to this file (chmod a+rw) 
$email = $_POST['user_email'];

// Future to do:
//   Implement a database for serving multiple users at the same time.
// The following overwrites existing content ($filename = user_email.txt)
// No database used. Tried playing around with SQLite and a little of MySQL
//   but decided to get a working solution first, saving user's email to .txt 
$fp = fopen($filename, "wb") or die('<!-- error: fopen failed -->
  <script>
  document.getElementById("uploadStatus").innerHTML=" \
    <div class=\"alert alert-error\"> \
      <h1>Upload failed.</h1> \
      <h2>Server error.</h2> \
      <h3>Please contact administrator to give write permissions to: \
          <ul><li>user_email.txt</li> \
              <li>upload_file.php</li></ul></h3> \
    </div> \
  ";
  </script>
  ');
if ($fp) {
  fwrite($fp, $email) or die('<!-- error: fwrite failed --> 
  <script>
  document.getElementById("uploadStatus").innerHTML=" \
    <div class=\"alert alert-error\"> \
      <h1>Upload failed.</h1> \
      <h2>Forgot to enter email address?</h2> \
      <h3>Please click the Back button on your browser and try again.</h3> \
    </div> \
  ";
  </script> 
 ');
  fclose($fp);
  echo "<!-- <p>Email saved</p> -->";
}
else {
  echo ' 
  <script>
  document.getElementById("uploadStatus").innerHTML=" \
    <div class=\"alert alert-error\"> \
      <h1>Upload failed.</h1> \
      <h2>Server error.</h2> \
      <h3>Please contact administrator to give write permissions to \
          upload_file.php and user_email.txt</h3> \
    </div> \
  ";
  </script> 
  ';
  echo "<!-- <p>Email NOT saved</p> -->";
}

//=============================================================================
// Saves user's sequence data
//=============================================================================
// Directory to upload to
$uploadDir = "upload/";

// To check for these filetype/extensions
$allowedExts = array("fasta", "fas", "fa", "ffn", "frn", "gbk", "gb", "txt");

$arrayFiles = $_FILES["file"]["name"];

if ($fileCount = count($arrayFiles))
  {
  for ($i = 0; $i < $fileCount; $i++)
    {
    // Extracts filetype / extension
    $extension = end(explode(".", $arrayFiles[$i]));

    if (($_FILES["file"]["size"][$i] < 100000000000) // afs 100GB project quota
      && in_array($extension, $allowedExts))
      {
      if (file_exists($uploadDir . $arrayFiles[$i]))
        {
        // Deletes file with same name.
        // Assumes one user at one time.
        //   Future to do:
        //     For serving multiple users using at the same time, we have to
        //     implementing a database to track uploaded sequences to specific
        //     users without deleting files with same name.
        unlink($uploadDir . $arrayFiles[$i]); 
        }
      move_uploaded_file($_FILES["file"]["tmp_name"][$i],
        $uploadDir . $arrayFiles[$i]);
      echo ' 
      <script>
      document.getElementById("uploadStatus").innerHTML=" \
        <div class=\"alert alert-success\"> \
          <h1>Upload complete.</h1> \
          <h2>You may now close this window & wait for email.</h2> \
          <h4>It will be from: 03713.project@gmail.com</h4> \
          <h4>&#160 &#160 &#160 &#160 &#160 &#160 &#160 &#160 titled: Job \
              Completed - A message from 03-713 Team B\'s web app</h4><br> \
        </div> \
      ";
      </script> 
      ';
//      echo "<p>Extra info: Stored in " . $uploadDir . $arrayFiles[$i] . "</p>";

      }
    else
      {
      echo ' 
      <script>
      document.getElementById("uploadStatus").innerHTML=" \
        <div class=\"alert alert-error\"> \
          <h1>Upload failed.</h1> \
          <h2>File sizes are over 100GB? No file selected?<br> \
              Or maybe uploading was interrupted.</h2> \
          <h3>Please click the Back button on your browser and try again.</h3> \
        </div> \
      ";
      </script> 
      ';
//      echo "<p>Error: File size over 100GB or uploading interrupted or no file
//               selected.</p>";
      }
    }
  }

// Testing error messages:
//  - invalid email format checked on index.html
//  - empty email field returns error in line 76
//  - empty file selection returns error in line 153
//  - user_email.txt without write permission returns error in line 62

?>

