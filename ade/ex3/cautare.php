<?php
if (!isset($_POST['nume'])) {
?>
<form method="post">
  Nume: <input type="text" name="nume"><br>
  Prenume: <input type="text" name="prenume"><br>
  <input type="submit" value="Cauta">
</form>
<?php
} else {
  $conn = OCILogon("student","student","XE");
  $stmt = OCIParse($conn,"SELECT * FROM angajati WHERE nume=:nume AND prenume=:prenume");
  OCIBindByName($stmt,":nume",$_POST['nume']);
  OCIBindByName($stmt,":prenume",$_POST['prenume']);
  OCIExecute($stmt);

  if (!OCIFetch($stmt)) {
      echo "Persoana nu exista!";
  } else {
      OCIExecute($stmt); // reexecutam pentru fetch complet
      echo "<table border='1'>";
      echo "<tr><th>ID</th><th>Nume</th><th>Prenume</th><th>An</th><th>Salar</th></tr>";
      while (OCIFetch($stmt)) {
          echo "<tr>";
          echo "<td>".OCIResult($stmt,"ID")."</td>";
          echo "<td>".OCIResult($stmt,"NUME")."</td>";
          echo "<td>".OCIResult($stmt,"PRENUME")."</td>";
          echo "<td>".OCIResult($stmt,"AN")."</td>";
          echo "<td>".OCIResult($stmt,"SALAR")."</td>";
          echo "</tr>";
      }
      echo "</table>";
  }

  OCIFreeStatement($stmt);
  OCILogoff($conn);
}
?>
