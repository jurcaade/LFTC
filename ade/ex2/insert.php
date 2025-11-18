<?php
if (!isset($_POST['titlu'])) {
?>
<form method="post">
  Nr crt: <input type="text" name="nr_crt"><br>
  Titlu: <input type="text" name="titlu"><br>
  Autor: <input type="text" name="autor"><br>
  Editura: <input type="text" name="editura"><br>
  Nr exemplare: <input type="text" name="exemplare"><br>
  An apariție: <input type="text" name="an"><br>
  ISBN: <input type="text" name="isbn"><br>
  <input type="submit" value="Adaugă">
</form>
<?php
} else {
  $conn = OCILogon("student","student","localhost:1521/XE");
  $stmt = OCIParse($conn,"INSERT INTO EVIDENTA_CARTI VALUES(:nr,:titlu,:autor,:editura,:exemplare,:an,:isbn)");
  OCIBindByName($stmt,":nr",$_POST['nr_crt']);
  OCIBindByName($stmt,":titlu",$_POST['titlu']);
  OCIBindByName($stmt,":autor",$_POST['autor']);
  OCIBindByName($stmt,":editura",$_POST['editura']);
  OCIBindByName($stmt,":exemplare",$_POST['exemplare']);
  OCIBindByName($stmt,":an",$_POST['an']);
  OCIBindByName($stmt,":isbn",$_POST['isbn']);
  OCIExecute($stmt,OCI_COMMIT_ON_SUCCESS);
  echo "Cartea a fost adăugată!";
  OCIFreeStatement($stmt);
  OCILogoff($conn);
}
?>
