<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "EVIDENTA";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) die("Eroare conexiune: " . $conn->connect_error);

$actiune = $_REQUEST['actiune'] ?? '';

echo "<a href='html2.html'>⬅️ Înapoi la meniu</a><hr>";

if ($actiune === 'listare') {
    $rez = $conn->query("SELECT * FROM EVIDENTA_CARTI");
    echo "<h2>Toate cărțile disponibile</h2>";
    echo "<table border='1'><tr><th>Nr</th><th>Titlu</th><th>Autor</th><th>Editura</th><th>ISBN</th><th>An</th><th>Stoc</th><th>Preț</th><th>Vândute</th><th>Total</th></tr>";
    while ($r = $rez->fetch_assoc()) {
        echo "<tr><td>{$r['NR_CRT']}</td><td>{$r['TITLU_CARTE']}</td><td>{$r['AUTOR_CARTE']}</td><td>{$r['EDITURA']}</td>
              <td>{$r['COD_ISBN']}</td><td>{$r['ANUL_APARITIEI']}</td><td>{$r['NR_EXEMPLARE_STOC']}</td><td>{$r['PRET']}</td>
              <td>{$r['NR_EXEMPLARE_VANDUTE']}</td><td>{$r['PRET_TOTAL_CARTE']}</td></tr>";
    }
    echo "</table>";

} elseif ($actiune === 'adaugare') {
    $titlu = $_POST['titlu'];
    $autor = $_POST['autor'];
    $editura = $_POST['editura'];
    $isbn = $_POST['isbn'];
    $an = $_POST['an'];
    $stoc = $_POST['stoc'];
    $pret = $_POST['pret'];

    $conn->query("INSERT INTO EVIDENTA_CARTI (TITLU_CARTE, AUTOR_CARTE, EDITURA, COD_ISBN, ANUL_APARITIEI, NR_EXEMPLARE_STOC, PRET)
                  VALUES ('$titlu', '$autor', '$editura', '$isbn', $an, $stoc, $pret)");
    echo "<h2>Cartea '$titlu' a fost adăugată cu succes!</h2>";

} elseif ($actiune === 'stergere') {
    $titlu = $_POST['titlu'];
    $rez = $conn->query("SELECT * FROM EVIDENTA_CARTI WHERE TITLU_CARTE='$titlu'");
    if ($rez->num_rows > 0) {
        $conn->query("DELETE FROM EVIDENTA_CARTI WHERE TITLU_CARTE='$titlu'");
        echo "<h2>Cartea '$titlu' a fost ștearsă.</h2>";
    } else {
        echo "<h2>Cartea '$titlu' nu există în baza de date.</h2>";
    }

} elseif ($actiune === 'vanzare') {
    $titlu = $_POST['titlu'];
    $nr_vandute = $_POST['nr_vandute'];
    $rez = $conn->query("SELECT * FROM EVIDENTA_CARTI WHERE TITLU_CARTE='$titlu'");
    if ($rez->num_rows == 0) {
        echo "<h2>Cartea '$titlu' nu există!</h2>";
    } else {
        $r = $rez->fetch_assoc();
        $stoc = $r['NR_EXEMPLARE_STOC'];
        $pret = $r['PRET'];
        $vandute = $r['NR_EXEMPLARE_VANDUTE'];

        if ($stoc >= $nr_vandute) {
            $stoc_nou = $stoc - $nr_vandute;
            $vandute_nou = $vandute + $nr_vandute;
            $pret_total = $vandute_nou * $pret;
            $conn->query("UPDATE EVIDENTA_CARTI SET NR_EXEMPLARE_STOC=$stoc_nou, NR_EXEMPLARE_VANDUTE=$vandute_nou, PRET_TOTAL_CARTE=$pret_total WHERE TITLU_CARTE='$titlu'");
            echo "<h2>Vânzare reușită pentru '$titlu'!</h2>";
        } else {
            echo "<h2>Nu sunt suficiente exemplare disponibile pentru '$titlu'!</h2>";
        }
    }

} elseif ($actiune === 'vandute') {
    $rez = $conn->query("SELECT * FROM EVIDENTA_CARTI WHERE NR_EXEMPLARE_VANDUTE > 0");
    $total_exemplare = 0;
    $total_incasari = 0;

    echo "<h2>Cărți vândute</h2>";
    echo "<table border='1'><tr><th>Nr</th><th>Titlu</th><th>Autor</th><th>Editura</th><th>Vândute</th><th>Total (lei)</th></tr>";
    while ($r = $rez->fetch_assoc()) {
        echo "<tr><td>{$r['NR_CRT']}</td><td>{$r['TITLU_CARTE']}</td><td>{$r['AUTOR_CARTE']}</td><td>{$r['EDITURA']}</td>
              <td>{$r['NR_EXEMPLARE_VANDUTE']}</td><td>{$r['PRET_TOTAL_CARTE']}</td></tr>";
        $total_exemplare += $r['NR_EXEMPLARE_VANDUTE'];
        $total_incasari += $r['PRET_TOTAL_CARTE'];
    }
    echo "</table>";
    echo "<h3>Total exemplare vândute: $total_exemplare</h3>";
    echo "<h3>Suma totală încasată: $total_incasari lei</h3>";
}

$conn->close();
?>

