<?php
$connection = OCILogon("student", "student", "");

$stmt = OCIParse($connection, "SELECT * FROM angajati");
OCIExecute($stmt);

echo "<table border='1' align='center'>";
echo "<tr><th>ID</th><th>NUME</th><th>SALAR</th></tr>";

while (OCIFetch($stmt)) {
    $id = OCIResult($stmt, "ID");
    $nume = OCIResult($stmt, "NUME");
    $salar = OCIResult($stmt, "SALAR");

    echo "<tr><td>$id</td><td>$nume</td><td>$salar</td></tr>";
}

echo "</table>";

OCIFreeStatement($stmt);
OCILogoff($connection);
?>
