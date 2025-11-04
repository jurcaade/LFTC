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

// Preluare acțiune
$actiune = $_POST['actiune'] ?? '';

if ($actiune === 'filtrare') {
    $media = floatval($_POST['media']);
    $rez = $conn->query("SELECT * FROM studenti WHERE media > $media");

    echo "<h2>Studenți cu media > $media</h2>";
    echo "<table border='1'><tr><th>ID</th><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Nota1</th><th>Nota2</th><th>Media</th></tr>";
    while ($row = $rez->fetch_assoc()) {
        echo "<tr>
                <td>{$row['id']}</td>
                <td>{$row['nume']}</td>
                <td>{$row['prenume']}</td>
                <td>{$row['grupa']}</td>
                <td>{$row['nota1']}</td>
                <td>{$row['nota2']}</td>
                <td>{$row['media']}</td>
              </tr>";
    }
    echo "</table>";

} elseif ($actiune === 'stergere') {
    $media = floatval($_POST['media']);
    $conn->query("DELETE FROM studenti WHERE media < $media");

    echo "<h2>Studenții rămași după ștergere</h2>";
    $rez = $conn->query("SELECT * FROM studenti");
    echo "<table border='1'><tr><th>ID</th><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Nota1</th><th>Nota2</th><th>Media</th></tr>";
    while ($row = $rez->fetch_assoc()) {
        echo "<tr>
                <td>{$row['id']}</td>
                <td>{$row['nume']}</td>
                <td>{$row['prenume']}</td>
                <td>{$row['grupa']}</td>
                <td>{$row['nota1']}</td>
                <td>{$row['nota2']}</td>
                <td>{$row['media']}</td>
              </tr>";
    }
    echo "</table>";

} elseif ($actiune === 'adaugare') {
    $nume = $conn->real_escape_string($_POST['nume']);
    $prenume = $conn->real_escape_string($_POST['prenume']);
    $grupa = intval($_POST['grupa']);
    $nota1 = floatval($_POST['nota1']);
    $nota2 = floatval($_POST['nota2']);
    $media = ($nota1 + $nota2) / 2;

    $conn->query("INSERT INTO studenti(nume, prenume, grupa, nota1, nota2, media)
                  VALUES('$nume', '$prenume', $grupa, $nota1, $nota2, $media)");

    echo "<h2>Student adăugat cu succes!</h2>";

    $rez = $conn->query("SELECT * FROM studenti");
    echo "<table border='1'><tr><th>ID</th><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Nota1</th><th>Nota2</th><th>Media</th></tr>";
    while ($row = $rez->fetch_assoc()) {
        echo "<tr>
                <td>{$row['id']}</td>
                <td>{$row['nume']}</td>
                <td>{$row['prenume']}</td>
                <td>{$row['grupa']}</td>
                <td>{$row['nota1']}</td>
                <td>{$row['nota2']}</td>
                <td>{$row['media']}</td>
              </tr>";
    }
    echo "</table>";

} elseif ($actiune === 'nepromovati') {
    // Studenți care nu au promovat (media < 5)
    $rez = $conn->query("SELECT * FROM studenti WHERE media < 5 ORDER BY grupa, nume");
    $nr = $rez->num_rows;

    echo "<h2>Număr studenți care nu au promovat laboratorul: $nr</h2>";

    if($nr > 0){
        echo "<table border='1'>
                <tr><th>Nume</th><th>Prenume</th><th>Grupa</th><th>Media</th></tr>";
        while($row = $rez->fetch_assoc()){
            echo "<tr>
                    <td>{$row['nume']}</td>
                    <td>{$row['prenume']}</td>
                    <td>{$row['grupa']}</td>
                    <td>{$row['media']}</td>
                  </tr>";
        }
        echo "</table>";
    } else {
        echo "<p>Toți studenții au promovat.</p>";
    }
}

$conn->close();
?>
