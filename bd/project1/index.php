<?php
    session_start();
?>
<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Home page</title>
<body>
    <?php
        if (!isset($_SESSION['Host'])) {
            $_SESSION['Host'] = 'localhost';
        }
        if (!isset($_SESSION['Db_name'])) {
            $_SESSION['Db_name'] = 'katerina';
        }
        if (!isset($_SESSION['Db_user'])) {
            $_SESSION['Db_user'] = 'root';
        }
        if (!isset($_SESSION['Db_pass'])) {
            $_SESSION['Db_pass'] = 'root';
        }
        $DB_HOST = $_SESSION['Host'];
        $DB_NAME = $_SESSION['Db_name'];
        $DB_USER = $_SESSION['Db_user'];
        $DB_PASSWORD = $_SESSION['Db_pass'];
    ?>

    <p>PHONE DIRECTORY</p>

    <form action="update_bd.php" method="get">
        <button type="submit">Update database</button>
    </form>

    <p>MENU:</p>
    <ul>
        <li><a href="companies.php"> View companies </a></li>
        <li><a href="people.php"> View people </a></li>
        <li><a href="find.php"> Find </a></li>
    </ul>
</body>
</html>