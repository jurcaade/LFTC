<?php
$conn = OCILogon("student","student","XE");
$stmt = OCIParse($conn,"SELECT * FROM angajati2 ORDER BY id");
OCIExecute($stmt);

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

OCIFreeStatement($stmt);
OCILogoff($conn);
?>
