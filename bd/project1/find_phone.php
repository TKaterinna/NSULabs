<?php
    session_start();
?>
<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Find phone</title>
<!-- номер телефона (обязательно)
город из списка выбрать (вариативно)
фио/название краткое -->
<body>
    <a href="index.php"> Home page </a><br>

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
    
    <script>
        function filterInput() {
            var container = document.getElementById('filter-form-container');
                
            var personIdSelect = document.createElement('select');
            personIdSelect.name = 'Person_id';

            var typePersonOption = document.createElement('option');
            typePersonOption.value = 'type';
            typePersonOption.textContent = 'Type name';
            personIdSelect.appendChild(typePersonOption);

            var allPeopleOption = document.createElement('option');
            allPeopleOption.value = 'all';
            allPeopleOption.textContent = 'All people';
            allPeopleOption.selected = 'true';
            personIdSelect.appendChild(allPeopleOption);

            var companyIdSelect = document.createElement('select');
            companyIdSelect.name = 'Company_id';

            var typeCompanyOption = document.createElement('option');
            typeCompanyOption.value = 'type';
            typeCompanyOption.textContent = 'Type name';
            companyIdSelect.appendChild(typeCompanyOption);

            var allCompaniesOption = document.createElement('option');
            allCompaniesOption.value = 'all';
            allCompaniesOption.textContent = 'All companies';
            allCompaniesOption.selected = 'true';
            companyIdSelect.appendChild(allCompaniesOption);

            <?php
                $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                if (!$pdo) {
                    echo 'error';
                } else {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $sql = "SELECT Company_id, Short_name FROM Companies";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Company_id']; ?>';
                            option.textContent = '<?php echo $row['Short_name']; ?>';
                            companyIdSelect.appendChild(option);
                            <?php
                        }
                    }

                    $sql = "SELECT Person_id, Last_name, First_name, Middle_name FROM People";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Person_id']; ?>';
                            option.textContent = '<?php echo $row['Last_name']; echo ' ' . $row['First_name']; echo ' ' . $row['Middle_name'];?>';
                            personIdSelect.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var inputInLastName = document.createElement('input');
            inputInLastName.type = 'text';
            inputInLastName.placeholder = 'In last name';
            inputInLastName.name = 'In_last_name';
            inputInLastName.style.display = 'none';

            var inputInFirstName = document.createElement('input');
            inputInFirstName.type = 'text';
            inputInFirstName.placeholder = 'In first name';
            inputInFirstName.name = 'In_first_name';
            inputInFirstName.style.display = 'none';

            var inputInMiddleName = document.createElement('input');
            inputInMiddleName.type = 'text';
            inputInMiddleName.placeholder = 'In middle name';
            inputInMiddleName.name = 'In_middle_name';
            inputInMiddleName.style.display = 'none';

            personIdSelect.onchange = function() {
                if (personIdSelect.value === 'type') {
                    inputInLastName.style.display = 'block';
                    inputInFirstName.style.display = 'block';
                    inputInMiddleName.style.display = 'block';
                } else {
                    inputInLastName.style.display = 'none';
                    inputInFirstName.style.display = 'none';
                    inputInMiddleName.style.display = 'none';
                }
            };

            var inputInShortName = document.createElement('input');
            inputInShortName.type = 'text';
            inputInShortName.placeholder = 'In short name';
            inputInShortName.name = 'In_short_name';
            inputInShortName.style.display = 'none';

            companyIdSelect.onchange = function() {
                if (companyIdSelect.value === 'type') {
                    inputInShortName.style.display = 'block';
                } else {
                    inputInShortName.style.display = 'none';
                }
            };

            var setButton = document.createElement('button');
            setButton.textContent = 'Set';
            setButton.onclick = function() {
                var inLastName = personIdSelect.value === 'type' ? inputInLastName.value : '';
                var inFirstName = personIdSelect.value === 'type' ? inputInFirstName.value : '';
                var inMiddleName = personIdSelect.value === 'type' ? inputInMiddleName.value : '';
                var inShortName = companyIdSelect.value === 'type' ? inputInShortName.value : '';

                var form = document.createElement('form');
                form.method = 'get';
                form.action = 'find_phone.php';

                var personIdInput = document.createElement('input');
                personIdInput.type = 'hidden';
                personIdInput.name = 'Person_id';
                personIdInput.value = personIdSelect.value;

                var companyIdInput = document.createElement('input');
                companyIdInput.type = 'hidden';
                companyIdInput.name = 'Company_id';
                companyIdInput.value = companyIdSelect.value;

                var inLastNameInput = document.createElement('input');
                inLastNameInput.type = 'hidden';
                inLastNameInput.name = 'In_last_name';
                inLastNameInput.value = inLastName;

                var inFirstNameInput = document.createElement('input');
                inFirstNameInput.type = 'hidden';
                inFirstNameInput.name = 'In_first_name';
                inFirstNameInput.value = inFirstName;

                var inMiddleNameInput = document.createElement('input');
                inMiddleNameInput.type = 'hidden';
                inMiddleNameInput.name = 'In_middle_name';
                inMiddleNameInput.value = inMiddleName;

                var inShortNameInput = document.createElement('input');
                inShortNameInput.type = 'hidden';
                inShortNameInput.name = 'In_short_name';
                inShortNameInput.value = inShortName;

                form.appendChild(personIdInput);
                form.appendChild(companyIdInput);
                form.appendChild(inLastNameInput);
                form.appendChild(inFirstNameInput);
                form.appendChild(inMiddleNameInput);
                form.appendChild(inShortNameInput);
                document.body.appendChild(form);
                form.submit();
            };

            var filtersText = document.createElement('p');
            filtersText.textContent = 'Filters:';

            container.appendChild(filtersText);
            container.appendChild(personIdSelect);
            container.appendChild(companyIdSelect);
            container.appendChild(inputInLastName);
            container.appendChild(inputInFirstName);
            container.appendChild(inputInMiddleName);
            container.appendChild(inputInShortName);
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

            $personID = isset($_GET['Person_id']) ? $_GET['Person_id'] : 'all';
            $companyID = isset($_GET['Company_id']) ? $_GET['Company_id'] : 'all';
            $inLastName = isset($_GET['In_last_name']) ? $_GET['In_last_name'] : '';
            $inFirstName = isset($_GET['In_first_name']) ? $_GET['In_first_name'] : '';
            $inMiddleName = isset($_GET['In_middle_name']) ? $_GET['In_middle_name'] : '';
            $inShortName = isset($_GET['In_short_name']) ? $_GET['In_short_name'] : '';

            if ($personID == 'all') {
                if ($companyID == 'all') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)";
                    $stmt = $pdo->prepare($sql);
                } else if ($companyID != 'type') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE p.Company_id = :companyID";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                } else {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE Short_name LIKE :inShortName";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindValue(':inShortName', '%' . $inShortName . '%', PDO::PARAM_STR);
                }
            } else if ($personID != 'type') {
                if ($companyID == 'all') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE p.Person_id = :personID";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                } else if ($companyID != 'type') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE (p.Company_id = :companyID) AND
                                (p.Person_id = :personID)";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                    $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                } else {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE (Short_name LIKE :inShortName) AND
                                (p.Person_id = :personID)";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindValue(':inShortName', '%' . $inShortName . '%', PDO::PARAM_STR);
                    $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                }
            } else {
                if ($companyID == 'all') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE (p.Last_name LIKE :inLastName) AND (p.First_name LIKE :inFirstName) AND (p.Middle_name LIKE :inMiddleName)";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindValue(':inLastName', '%' . $inLastName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inFirstName', '%' . $inFirstName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inMiddleName', '%' . $inMiddleName . '%', PDO::PARAM_STR);
                } else if ($companyID != 'type') {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE (p.Company_id = :companyID) AND
                                (p.Last_name LIKE :inLastName) AND (p.First_name LIKE :inFirstName) AND (p.Middle_name LIKE :inMiddleName)";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                    $stmt->bindValue(':inLastName', '%' . $inLastName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inFirstName', '%' . $inFirstName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inMiddleName', '%' . $inMiddleName . '%', PDO::PARAM_STR);
                } else {
                    $sql = "SELECT p.Person_id, Last_name, First_name, Middle_name, Short_name, Phone_id, Phone_number, Phone_code
                            FROM People p
                            LEFT JOIN Phones ph ON (p.Person_id = ph.Person_id)
                            LEFT JOIN Companies c ON (p.Company_id = c.Company_id)
                            LEFT JOIN Cities city ON (ph.City_id = city.City_id)
                            WHERE (Short_name LIKE :inShortName) AND
                                (p.Last_name LIKE :inLastName) AND (p.First_name LIKE :inFirstName) AND (p.Middle_name LIKE :inMiddleName)";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindValue(':inShortName', '%' . $inShortName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inLastName', '%' . $inLastName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inFirstName', '%' . $inFirstName . '%', PDO::PARAM_STR);
                    $stmt->bindValue(':inMiddleName', '%' . $inMiddleName . '%', PDO::PARAM_STR);
                }
            }
                
            $stmt->execute();
                
            if ($stmt->rowCount() > 0) {
                echo "<p>Phones:</p>";
                echo "<table border='1'>
                        <tr>
                            <th>Name</th>
                            <th>Company_name</th>
                            <th>Phone</th>
                        </tr>";
        
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                    echo "<tr>
                            <td><a href= \"person.php?Person_id=  ${row['Person_id']} \"> ${row['Last_name']}  ${row['First_name']}  ${row['Middle_name']} </a></td>
                            <td>" . $row["Short_name"] . "</td>
                            <td>(" . $row["Phone_code"] . ")". $row["Phone_number"] . "</td>
                        </tr>";
                }
                echo "</table>";
            } else {
                echo "table is empty";
            }
        };
    ?><br>
</body>
</html>