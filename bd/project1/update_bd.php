<?php
    session_start();
?>
<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Update database</title>
<body>
<script>
    function fiterInput() {
        var container = document.getElementById('filterform-container');

        var host = document.createElement('input');
        host.value = 'localhost';
        host.required = true;
        var dbName = document.createElement('input');
        dbName.value = 'katerina';
        dbName.required = true;
        var dbUser = document.createElement('input');
        dbUser.value = 'root';
        dbUser.required = true;
        var dbPass = document.createElement('input');
        dbPass.value = 'root';
        dbPass.required = true;
    
        var udateButton = document.createElement('button');
        udateButton.textContent = 'Update';
        udateButton.onclick = function() {
            if (host.value.trim() === '') {
                event.preventDefault();
                alert('Please enter a host');
                return false;
            } else if (dbName.value.trim() === '') {
                event.preventDefault();
                alert('Please enter a database name');
                return false;
            } else if (dbUser.value.trim() === '') {
                event.preventDefault();
                alert('Please enter a user name');
                return false;
            } else if (dbPass.value.trim() === '') {
                event.preventDefault();
                alert('Please enter a password');
                return false;
            }

            var form = document.createElement('form');
            form.method = 'post';
            form.action = 'update_bd.php';

            var hostInput = document.createElement('input');
            hostInput.type = 'hidden';
            hostInput.name = 'Host';
            hostInput.value = host.value;

            var dbNameInput = document.createElement('input');
            dbNameInput.type = 'hidden';
            dbNameInput.name = 'Db_name';
            dbNameInput.value = dbName.value;

            var dbUserInput = document.createElement('input');
            dbUserInput.type = 'hidden';
            dbUserInput.name = 'Db_user';
            dbUserInput.value = dbUser.value;

            var dbPassInput = document.createElement('input');
            dbPassInput.type = 'hidden';
            dbPassInput.name = 'Db_pass';
            dbPassInput.value = dbPass.value;
                
            form.appendChild(hostInput);
            form.appendChild(dbNameInput);
            form.appendChild(dbUserInput);
            form.appendChild(dbPassInput);
            document.body.appendChild(form);
            form.submit();
        };

        var hostText = document.createElement('p');
        hostText.textContent = 'Host:';
        var dbNameText = document.createElement('p');
        dbNameText.textContent = 'Database name:';
        var dbUserText = document.createElement('p');
        dbUserText.textContent = 'User name:';
        var dbPassText = document.createElement('p');
        dbPassText.textContent = 'Password:';

        container.appendChild(hostText);
        container.appendChild(host);
        container.appendChild(dbNameText);
        container.appendChild(dbName);
        container.appendChild(dbUserText);
        container.appendChild(dbUser);
        container.appendChild(dbPassText);
        container.appendChild(dbPass);
        container.appendChild(udateButton);
    }
    </script>
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
        
        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
            if (empty($_POST['Host']) || empty($_POST['Db_name']) || empty($_POST['Db_user']) || empty($_POST['Db_pass'])) {
                echo "ERROR: no parameters";
            } else {
                $DB_HOST = $_POST['Host'];
                $DB_NAME = $_POST['Db_name'];
                $DB_USER = $_POST['Db_user'];
                $DB_PASSWORD = $_POST['Db_pass'];

                $_SESSION['Host'] = $DB_HOST;
                $_SESSION['Db_name'] = $DB_NAME;
                $_SESSION['Db_user'] = $DB_USER;
                $_SESSION['Db_pass'] = $DB_PASSWORD;

                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $pdo->exec("DROP DATABASE IF EXISTS " . $DB_NAME);
                    $pdo->exec("CREATE DATABASE " . $DB_NAME);

                    echo "The database have been successfully created\n";
                } catch (PDOException $e) {
                    echo "ERROR: " . $e->getMessage();
                }

                echo "<br></br>";

                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $pdo->exec("
                        CREATE TABLE People (
                            Person_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Last_name TEXT NOT NULL,
                            First_name TEXT NOT NULL,
                            Middle_name TEXT NOT NULL,
                            Company_ID INT,
                            Position_ID INT
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Companies (
                            Company_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Short_name TEXT NOT NULL,
                            Full_name TEXT NOT NULL,
                            Address_ID INT NOT NULL
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Specializations (
                            Sp_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Name VARCHAR(255) NOT NULL UNIQUE
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Cities (
                            City_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Name VARCHAR(255) NOT NULL UNIQUE,
                            Phone_code INT UNIQUE CHECK (Phone_code BETWEEN 100 AND 999),
                            Post_code INT UNIQUE CHECK (Post_code BETWEEN 100 AND 999)
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Phones (
                            Phone_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Phone_number VARCHAR(7) NOT NULL CHECK (LENGTH(Phone_number) = 7),
                            City_ID INT NOT NULL,
                            Person_ID INT NOT NULL
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Addresses (
                            Address_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            City_ID INT NOT NULL,
                            Street TEXT NOT NULL,
                            House TEXT NOT NULL,
                            Post_office_code INT NOT NULL CHECK (Post_office_code BETWEEN 100 AND 999)
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Positions (
                            Position_ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Name VARCHAR(255) NOT NULL UNIQUE
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Specializations_of_people (
                            ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Sp_ID INT NOT NULL,
                            Person_ID INT NOT NULL,
                            UNIQUE (Sp_ID, Person_ID)
                        )
                    ");

                    $pdo->exec("
                        CREATE TABLE Specializations_of_companies (
                            ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
                            Sp_ID INT NOT NULL,
                            Company_ID INT NOT NULL,
                            UNIQUE (Sp_ID, Company_ID)
                        )
                    ");

                    $pdo->exec("
                        INSERT INTO Cities (Name, Phone_code, Post_code) VALUES
                        ('Москва', 101, 654),
                        ('Санкт-Петербург', 102, 736),
                        ('Новосибирск', 103, 983),
                        ('Екатеринбург', 104, 374),
                        ('Казань', 105, 734),
                        ('Нижний Новгород', 106, 176),
                        ('Самара', 107, 965)
                    ");

                    $pdo->exec("
                        INSERT INTO Addresses (City_ID, Street, House, Post_office_code) VALUES
                        (1, 'Ленина', '10', 847),
                        (3, 'Невский проспект', '25', 546),
                        (6, 'Советская', '5', 542),
                        (2, 'Пушкина', '15', 987),
                        (4, 'Баумана', '30', 940),
                        (1, 'Большая Покровская', '12', 342),
                        (5, 'Московское шоссе', '45', 982)
                    ");

                    $pdo->exec("
                        INSERT INTO Companies (Short_name, Full_name, Address_ID) VALUES
                        ('Газпром', 'ПАО \"Газпром\"', 1),
                        ('Лукойл', 'ОАО \"Лукойл\"', 2),
                        ('Роснефть', 'ПАО \"Роснефть\"', 3),
                        ('Сбербанк', 'ПАО \"Сбербанк России\"', 4),
                        ('ГМК Норильский Никель', 'ПАО \"ГМК Норильский Никель\"', 5),
                        ('Магнит', 'ПАО \"Магнит\"', 6),
                        ('МТС', 'ПАО \"Мобильные ТелеСистемы\"', 7)
                    ");

                    $pdo->exec("
                        INSERT INTO Positions (Name) VALUES
                        ('Менеджер'),
                        ('Разработчик'),
                        ('Дизайнер'),
                        ('Аналитик'),
                        ('Тестировщик'),
                        ('Инженер'),
                        ('Консультант')
                    ");

                    $pdo->exec("
                        INSERT INTO People (Last_name, First_name, Middle_name, Company_ID, Position_ID) VALUES
                        ('Иванов', 'Александр', 'Петрович', 1, 1),
                        ('Петров', 'Дмитрий', 'Сергеевич', 2, 2),
                        ('Сидоров', 'Иван', 'Иванович', NULL, NULL),
                        ('Смирнов', 'Алексей', 'Алексеевич', 4, 4),
                        ('Кузнецов', 'Дмитрий', 'Дмитриевич', 5, 5),
                        ('Васильев', 'Василий', 'Васильевич', 6, 6),
                        ('Михайлов', 'Михаил', 'Михайлович', 7, 3)
                    ");

                    $pdo->exec("
                        INSERT INTO Specializations (Name) VALUES
                        ('Поставка нефти'),
                        ('Поставка одежды'),
                        ('Выпуск металлопроката'),
                        ('Банковские услуги'),
                        ('Горнодобывающая промышленность'),
                        ('Розничная торговля'),
                        ('Телекоммуникации')
                    ");

                    $pdo->exec("
                        INSERT INTO Specializations_of_people (Sp_ID, Person_ID) VALUES
                        (1, 1),
                        (2, 2),
                        (3, 4),
                        (4, 7),
                        (5, 5),
                        (6, 3),
                        (7, 6),
                        (1, 2)
                    ");

                    $pdo->exec("
                        INSERT INTO Specializations_of_companies (Sp_ID, Company_ID) VALUES
                        (1, 5),
                        (2, 2),
                        (3, 3),
                        (4, 1),
                        (3, 1),
                        (5, 4),
                        (6, 7),
                        (7, 2)
                    ");

                    $pdo->exec("
                        INSERT INTO Phones (Phone_number, City_ID, Person_ID) VALUES
                        ('1234567', 1, 4),
                        ('2345678', 2, 1),
                        ('3456789', 3, 2),
                        ('4567890', 4, 6),
                        ('5678901', 5, 3),
                        ('6789012', 6, 5),
                        ('7890123', 7, 7),
                        ('7890321', 2, 1)
                    ");

                    $pdo->exec("
                        ALTER TABLE People
                        ADD CONSTRAINT fk_company_for_person FOREIGN KEY (Company_ID) REFERENCES Companies(Company_ID),
                        ADD CONSTRAINT fk_position_for_person FOREIGN KEY (Position_ID) REFERENCES Positions(Position_ID)
                    ");

                    $pdo->exec("
                        ALTER TABLE Companies
                        ADD CONSTRAINT fk_address_for_company FOREIGN KEY (Address_ID) REFERENCES Addresses(Address_ID)
                    ");

                    $pdo->exec("
                        ALTER TABLE Phones
                        ADD CONSTRAINT fk_city_for_phone FOREIGN KEY (City_ID) REFERENCES Cities(City_ID),
                        ADD CONSTRAINT fk_person_for_phone FOREIGN KEY (Person_ID) REFERENCES People(Person_ID)
                    ");

                    $pdo->exec("
                        ALTER TABLE Addresses
                        ADD CONSTRAINT fk_city_for_address FOREIGN KEY (City_ID) REFERENCES Cities(City_ID)
                    ");

                    $pdo->exec("
                        ALTER TABLE Specializations_of_people
                        ADD CONSTRAINT fk_specialization_for_person FOREIGN KEY (Sp_ID) REFERENCES Specializations(Sp_ID),
                        ADD CONSTRAINT fk_person_for_specialization FOREIGN KEY (Person_ID) REFERENCES People(Person_ID)
                    ");

                    $pdo->exec("
                        ALTER TABLE Specializations_of_companies
                        ADD CONSTRAINT fk_specialization_for_company FOREIGN KEY (Sp_ID) REFERENCES Specializations(Sp_ID),
                        ADD CONSTRAINT fk_company_for_specialization FOREIGN KEY (Company_ID) REFERENCES Companies(Company_ID)
                    ");

                    $pdo->exec("
                        CREATE TRIGGER before_insert_phone_number
                        BEFORE INSERT ON Phones
                        FOR EACH ROW
                        BEGIN
                            IF NEW.Phone_number NOT REGEXP '^[0-9]+$' THEN
                                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Phone number must contain only digits';
                            END IF;
                        END;
                    ");

                    $pdo->exec("
                        CREATE TRIGGER before_update_phone_number
                        BEFORE UPDATE ON Phones
                        FOR EACH ROW
                        BEGIN
                            IF NEW.Phone_number NOT REGEXP '^[0-9]+$' THEN
                                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Phone number must contain only digits';
                            END IF;
                        END;
                    ");

                    echo "The tables have been successfully created and filled with data\n";
                } catch (PDOException $e) {
                    echo "ERROR: " . $e->getMessage();
                }
            }

            echo "<form id='redirectForm' action='index.php' method='get'>
                        <button type='submit'>Return</button>
                    </form>";
            echo "<script>
                    setTimeout(function() { document.getElementById('redirectForm').submit(); }, 20000);
                </script>";
        } else {
            echo "Parameters:<br>";
            echo "<div id='filterform-container'></div><br><br>";
            ?>
            <script>
                fiterInput();
            </script>
            <?php
                echo "<form id='redirectForm' action='index.php' method='get'>
                            <button type='submit'>Return</button>
                        </form>";
        }
    ?>
</body>
</html>