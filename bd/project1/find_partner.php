<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Find partner</title>
<!-- тут нужно галочками проставить фирму и человека
специализация из списка   -->
<body>
    <a href="index.php"> Home page </a><br>

    <?php
        session_start();
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

    <script>
        function filterInput() {
            var container = document.getElementById('filter-form-container');
                
            var typePartnerSelect = document.createElement('select');
            typePartnerSelect.name = 'Type_partner';

            var bothOption = document.createElement('option');
            bothOption.value = 'both';
            bothOption.textContent = 'Companies and people';
            typePartnerSelect.appendChild(bothOption);

            var companiesOption = document.createElement('option');
            companiesOption.value = 'companies';
            companiesOption.textContent = 'Companies';
            typePartnerSelect.appendChild(companiesOption);

            var peopleOption = document.createElement('option');
            peopleOption.value = 'people';
            peopleOption.textContent = 'People';
            typePartnerSelect.appendChild(peopleOption);

            var spIdSelect = document.createElement('select');
            spIdSelect.name = 'Sp_id';

            var allOption = document.createElement('option');
            allOption.value = 'all';
            allOption.textContent = 'All specializations';
            allOption.selected = 'true';
            spIdSelect.appendChild(allOption);

            <?php
                $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                if (!$pdo) {
                    echo 'error';
                } else {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $sql = "SELECT Sp_id, Name FROM Specializations";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Sp_id']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
                            spIdSelect.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var setButton = document.createElement('button');
            setButton.textContent = 'Set';
            setButton.onclick = function() {
                var form = document.createElement('form');
                form.method = 'get';
                form.action = 'find_partner.php';

                var typePartnerInput = document.createElement('input');
                typePartnerInput.type = 'hidden';
                typePartnerInput.name = 'Type_partner';
                typePartnerInput.value = typePartnerSelect.value;

                var spIdInput = document.createElement('input');
                spIdInput.type = 'hidden';
                spIdInput.name = 'Sp_id';
                spIdInput.value = spIdSelect.value;

                form.appendChild(typePartnerInput);
                form.appendChild(spIdInput);
                document.body.appendChild(form);
                form.submit();
            };

            var filtersText = document.createElement('p');
            filtersText.textContent = 'Filters:';

            container.appendChild(typePartnerSelect);
            container.appendChild(spIdSelect);
            container.appendChild(setButton);
        }
    </script>

    <?php
        $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME , $DB_USER, $DB_PASSWORD);
        if (!$pdo) {
            echo 'error';
        } else {
            echo "<div id='filter-form-container'></div><br><br>";
            ?>
            <script>
                filterInput();
            </script>
            <?php
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
            $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

            $typePartner = isset($_GET['Type_partner']) ? $_GET['Type_partner'] : 'both';
            $spID = isset($_GET['Sp_id']) ? $_GET['Sp_id'] : 'all';

            if ($typePartner != 'companies') {
                if ($spID == 'all') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name
                            FROM People p
                            LEFT JOIN Specializations_of_people sp ON (p.Person_id = sp.Person_id)
                            GROUP BY p.Person_id, Last_name, First_name, Middle_name";
                    $stmt = $pdo->prepare($sql);
                } else {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name
                            FROM People p
                            LEFT JOIN Specializations_of_people sp ON (p.Person_id = sp.Person_id)
                            WHERE Sp_id = :spID
                            GROUP BY p.Person_id, Last_name, First_name, Middle_name";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                }
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<p>People:</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<li>
                                <a href= \"person.php?Person_id=  ${row['Person_id']} \"> ${row['Last_name']}  ${row['First_name']}  ${row['Middle_name']} </a>
                            </li>";
                    }
                    echo '</ol>';
                } else {
                    echo "table is empty";
                }
            }
            if ($typePartner != 'people') {
                if ($spID == 'all') {
                    $sql = "SELECT c.Company_id, Short_name
                            FROM Companies c
                            LEFT JOIN Specializations_of_companies sp ON (c.Company_id = sp.Company_id)
                            GROUP BY c.Company_id, Short_name";
                    $stmt = $pdo->prepare($sql);
                } else {
                    $sql = "SELECT c.Company_id, Short_name
                            FROM Companies c
                            LEFT JOIN Specializations_of_companies sp ON (c.Company_id = sp.Company_id)
                            WHERE Sp_id = :spID
                            GROUP BY c.Company_id, Short_name";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                }
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<p>Companies:</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<li>
                                <a href= \"company.php?Company_id=  ${row['Company_id']} \"> ${row['Short_name']}</a>
                            </li>";
                    }
                    echo '</ol>';
                } else {
                    echo "table is empty";
                }
            }
        };
    ?><br>
</body>
</html>