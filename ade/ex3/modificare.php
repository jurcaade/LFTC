<?php
if (!isset($_POST['nume'])) {
?>
<form method="post">
  Nume: <input type="text" name="nume"><br>
  Prenume: <input type="text" name="prenume"><br>
  <input type="submit" value="Modifica">
</form>
<?php
} else {
  $conn = OCILogon("student","student","XE");

  // calculam nr ani si media
  $stmt = OCIParse($conn,"SELECT COUNT(*) AS NRANI, AVG(salar) AS MEDIE 
                          FROM angajati 
                          WHERE nume=:nume AND prenume=:prenume");
  OCIBindByName($stmt,":nume",$_POST['nume']);
  OCIBindByName($stmt,":prenume",$_POST['prenume']);
  OCIExecute($stmt);

  if (OCIFetch($stmt)) {
      $nrani = OCIResult($stmt,"NRANI");
      $medie = OCIResult($stmt,"MEDIE");

      if ($nrani == 0) {
          echo "Persoana nu exista!";
      } else {
          // stergem toate inregistrarile persoanei
          $stmt2 = OCIParse($conn,"DELETE FROM angajati WHERE nume=:nume AND prenume=:prenume");
          OCIBindByName($stmt2,":nume",$_POST['nume']);
          OCIBindByName($stmt2,":prenume",$_POST['prenume']);
          OCIExecute($stmt2,OCI_COMMIT_ON_SUCCESS);

          // inseram o singura inregistrare cu nr ani si media
          $stmt3 = OCIParse($conn,"INSERT INTO angajati(id,nume,prenume,an,salar) 
                                   VALUES(seq_angajati.NEXTVAL,:nume,:prenume,:nrani,:medie)");
          OCIBindByName($stmt3,":nume",$_POST['nume']);
          OCIBindByName($stmt3,":prenume",$_POST['prenume']);
          OCIBindByName($stmt3,":nrani",$nrani);
          OCIBindByName($stmt3,":medie",$medie);
          OCIExecute($stmt3,OCI_COMMIT_ON_SUCCESS);

          // afisam rezultatul cu campurile pe verticala
          echo "<table border='1'>";
          echo "<tr><td>Nume</td><td>".$_POST['nume']."</td></tr>";
          echo "<tr><td>Prenume</td><td>".$_POST['prenume']."</td></tr>";
          echo "<tr><td>Nr ani</td><td>".$nrani."</td></tr>";
          echo "<tr><td>Media salar</td><td>".$medie."</td></tr>";
          echo "</table>";

          OCIFreeStatement($stmt3);
          OCIFreeStatement($stmt2);
      }
  }

  OCIFreeStatement($stmt);
  OCILogoff($conn);
}
?>
