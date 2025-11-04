<?php
$servername = "localhost";
$username = "root";
$password = "";  // modifică dacă ai parolă la MySQL
$dbname = "evidenta";

// Conectare
$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Eroare conexiune: " . $conn->connect_error);
}

// Actualizare câmp medie pentru toate înregistrările
$conn->query("UPDATE studenti SET media = (nota1 + nota2)/2");

$actiune = $_POST['actiune'] ?? '';

if ($actiune === 'filtrare') {
    $media = $_POST['media'];
    $rez = $conn->query("SELECT * FROM studenti WHERE media > $media");
    echo "<h2>Studenți cu media > $media</h2>";
    echo "<table border='1'><tr><th>ID</th><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Nota1</th><th>Nota2</th><th>Media</th></tr>";
    while ($row = $rez->fetch_assoc()) {
        echo "<tr><td>{$row['id']}</td><td>{$row['nume']}</td><td>{$row['prenume']}</td><td>{$row['grupa']}</td>
              <td>{$row['nota1']}</td><td>{$row['nota2']}</td><td>{$row['media']}</td></tr>";
    }
    echo "</table>";

} elseif ($actiune === 'stergere') {
    $media = $_POST['media'];
    $conn->query("DELETE FROM studenti WHERE media < $media");
    echo "<h2>Studenții rămași după ștergere</h2>";
    $rez = $conn->query("SELECT * FROM studenti");
    echo "<table border='1'><tr><th>ID</th><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Nota1</th><th>Nota2</th><th>Media</th></tr>";
    while ($row = $rez->fetch_assoc()) {
        echo "<tr><td>{$row['id']}</td><td>{$row['nume']}</td><td>{$row['prenume']}</td><td>{$row['grupa']}</td>
              <td>{$row['nota1']}</td><td>{$row['nota2']}</td><td>{$row['media']}</td></tr>";
    }
    echo "</table>";

} elseif ($actiune === 'adaugare') {
    $nume = $_POST['nume'];
    $prenume = $_POST['prenume'];
    $grupa = $_POST['grupa'];
    $nota1 = $_POST['nota1'];
    $nota2 = $_POST['nota2'];
    $media = ($nota1 + $nota2) / 2;
    $conn->query("INSERT INTO studenti(nume, prenume, grupa, nota1, nota2, media)
                  VALUES('$nume', '$prenume', $grupa, $nota1, $nota2, $media)");
    echo "<h2>Student adăugat cu succes!</h2>";
    $rez = $conn->query("SELECT * FROM studenti");
    echo "<table border='1'><tr><th>ID</th><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Nota1</th><th>Nota2</th><th>Media</th></tr>";
    while ($row = $rez->fetch_assoc()) {
        echo "<tr><td>{$row['id']}</td><td>{$row['nume']}</td><td>{$row['prenume']}</td><td>{$row['grupa']}</td>
              <td>{$row['nota1']}</td><td>{$row['nota2']}</td><td>{$row['media']}</td></tr>";
    }
    echo "</table>";
}

$conn->close();
?>