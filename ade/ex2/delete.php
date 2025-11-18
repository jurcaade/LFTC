<?php
if (!isset($_POST['titlu'])) {
?>
<form method="post" onsubmit="return confirm('Sigur vrei să ștergi această carte?');">
  Titlu carte: <input type="text" name="titlu"><br>
  <input type="submit" value="Șterge">
</form>
<?php
} else {
  $conn = OCILogon("student","student","XE");
  $stmt = OCIParse($conn,"DELETE FROM EVIDENTA_CARTI WHERE TITLU_CARTE=:titlu");
  OCIBindByName($stmt,":titlu",$_POST['titlu']);
  OCIExecute($stmt,OCI_COMMIT_ON_SUCCESS);
  echo "Cartea a fost ștearsă (dacă exista).";
  OCIFreeStatement($stmt);
  OCILogoff($conn);
}
?>
