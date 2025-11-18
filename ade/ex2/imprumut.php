<?php
if (!isset($_POST['titlu'])) {
?>
<form method="post">
  Titlu carte: <input type="text" name="titlu"><br>
  <input type="submit" value="Împrumută">
</form>
<?php
} else {
  $conn = OCILogon("student","student","XE");

  // verificăm dacă există cartea
  $stmt = OCIParse($conn,"SELECT NR_EXEMPLARE FROM EVIDENTA_CARTI WHERE TITLU_CARTE=:titlu");
  OCIBindByName($stmt,":titlu",$_POST['titlu']);
  OCIExecute($stmt);
  if (OCIFetch($stmt)) {
    $nr = OCIResult($stmt,"NR_EXEMPLARE");
    if ($nr > 0) {
      $stmt2 = OCIParse($conn,"UPDATE EVIDENTA_CARTI SET NR_EXEMPLARE=NR_EXEMPLARE-1 WHERE TITLU_CARTE=:titlu");
      OCIBindByName($stmt2,":titlu",$_POST['titlu']);
      OCIExecute($stmt2,OCI_COMMIT_ON_SUCCESS);
      echo "Împrumut reușit! Mai sunt ".($nr-1)." exemplare.";
      OCIFreeStatement($stmt2);
    } else {
      echo "Nu mai sunt exemplare disponibile!";
    }
  } else {
    echo "Cartea nu există!";
  }
  OCIFreeStatement($stmt);
  OCILogoff($conn);
}
?>
