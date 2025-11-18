<?php
if (!isset($_POST['nume'])) {
?>
<form method="post">
  Nume: <input type="text" name="nume"><br>
  Prenume: <input type="text" name="prenume"><br>
  An: <input type="text" name="an"><br>
  Salar: <input type="text" name="salar"><br>
  <input type="submit" value="Salveaza">
</form>
<?php
} else {
  $conn = OCILogon("student","student","XE");

  // verificam daca exista persoana + anul
  $stmt = OCIParse($conn,"SELECT id FROM angajati WHERE nume=:nume AND prenume=:prenume AND an=:an");
  OCIBindByName($stmt,":nume",$_POST['nume']);
  OCIBindByName($stmt,":prenume",$_POST['prenume']);
  OCIBindByName($stmt,":an",$_POST['an']);
  OCIExecute($stmt);

  if (OCIFetch($stmt)) {
      // update
      $stmt2 = OCIParse($conn,"UPDATE angajati SET salar=:salar WHERE nume=:nume AND prenume=:prenume AND an=:an");
      OCIBindByName($stmt2,":salar",$_POST['salar']);
      OCIBindByName($stmt2,":nume",$_POST['nume']);
      OCIBindByName($stmt2,":prenume",$_POST['prenume']);
      OCIBindByName($stmt2,":an",$_POST['an']);
      OCIExecute($stmt2,OCI_COMMIT_ON_SUCCESS);
      echo "Inregistrarea a fost actualizata.";
      OCIFreeStatement($stmt2);
  } else {
      // insert
      $stmt2 = OCIParse($conn,"INSERT INTO angajati(id,nume,prenume,an,salar) VALUES(seq_angajati.NEXTVAL,:nume,:prenume,:an,:salar)");
      OCIBindByName($stmt2,":nume",$_POST['nume']);
      OCIBindByName($stmt2,":prenume",$_POST['prenume']);
      OCIBindByName($stmt2,":an",$_POST['an']);
      OCIBindByName($stmt2,":salar",$_POST['salar']);
      OCIExecute($stmt2,OCI_COMMIT_ON_SUCCESS);
      echo "Inregistrarea a fost inserata.";
      OCIFreeStatement($stmt2);
  }

  OCIFreeStatement($stmt);
  OCILogoff($conn);
}
?>
