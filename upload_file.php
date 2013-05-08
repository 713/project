<?php
//=============================================================================
// Save user email to file first. Then, upload user's data.
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
  <link href="css/bootstrap.min.css" rel="stylesheet" media="screen" type="text/css" />
</head>

<body>
  <br>
  <div class="container">
    <div class="hero-unit">
      <h1>Team B's 03-713 project</h1>
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
// Saves user email
//=============================================================================
$filename = "user_email.txt"; // Need chmod a+rw
$email = $_POST['user_email'];

$fp = fopen($filename, "wb") or die('error: fopen failed'); // Discards existing content
if ($fp) {
  fwrite($fp, $email) or die('error: fwrite failed');
  fclose($fp);
  echo "<p>Email saved</p>";
}
else {
  echo "<p>Email NOT saved</p>";
}

//=============================================================================
// Saves user's data
//=============================================================================
// Directory to upload to
$uploadDir = "upload/";

// To check for filetype/extensions
$allowedExts = array("fasta", "fas", "fa", "ffn", "frn", "txt");

// Alias/short-hand
$arrayFiles = $_FILES["file"]["name"];

if ($fileCount = count($arrayFiles))
  {
  for ($i = 0; $i < $fileCount; $i++)
    {
    // Extracts filetype / extension
    $extension = end(explode(".", $arrayFiles[$i]));

    if (($_FILES["file"]["size"][$i] < 100000000000)  // afs: 100GB quota for projects
      && in_array($extension, $allowedExts))
      {
      if (file_exists($uploadDir . $arrayFiles[$i]))
        {
        // Deletes file with same name. Assumes old file is not in use.
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
          <h4>&#160 &#160 &#160 &#160 &#160 &#160 &#160 &#160 titled: Job Completed - A message from 03-713 Team B\'s web app</h4><br> \
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
          <h2>Invalid filetype or file size is over 100GB.</h2> \
          <h3>Please click the Back button on your browser and try again?</h3> \
        </div> \
      ";
      </script> 
      ';
      }
    }
  }

// Testing
//echo count($arrayFiles);
//echo $_FILES["file"];
//echo $_FILES["file"]["name"];
//echo $_FILES["file"]["name"][0];
//echo in_array(end(explode(".", $arrayFiles[0])), $allowedExts);

?>

