<?php
// Directory to upload to
$uploadDir = "upload/";

// To check for filetype/extensions
$allowedExts = array("fasta", "fas", "fa", "txt");

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
      echo "<p> Stored in: " . $uploadDir . $arrayFiles[$i] . "</p>";
      }
    else
      {
      echo "<p>Invalid filetype or file size is over 100GB</p>";
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

