<?php
$conn = OCILogon("student","student","");

$stmt = OCIParse($conn, "SELECT * FROM EVIDENTA_CARTI");
OCIExecute($stmt);

echo "<table border='1' align='center'>";
echo "<tr><th>NR_CRT</th><th>TITLU</th><th>AUTOR</th><th>EDITURA</th><th>EXEMPLARE</th><th>AN</th><th>ISBN</th></tr>";

while (OCIFetch($stmt)) {
    echo "<tr>";
    echo "<td>".OCIResult($stmt,"NR_CRT")."</td>";
    echo "<td>".OCIResult($stmt,"TITLU_CARTE")."</td>";
    echo "<td>".OCIResult($stmt,"AUTOR_CARTE")."</td>";
    echo "<td>".OCIResult($stmt,"EDITURA")."</td>";
    echo "<td>".OCIResult($stmt,"NR_EXEMPLARE")."</td>";
    echo "<td>".OCIResult($stmt,"ANUL_APARITIEI")."</td>";
    echo "<td>".OCIResult($stmt,"COD_ISBN")."</td>";
    echo "</tr>";
}
echo "</table>";

OCIFreeStatement($stmt);
OCILogoff($conn);
?>
