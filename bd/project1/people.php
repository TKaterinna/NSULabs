<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>People</title>
<!-- список людей их фио, количество специализаций + кнопка удаления + все ссылками на личные страницы
сортировка по фио, количеству специализаций
фильтр по имени/фамилии/отчетсву
кнопка добавления партнера -->
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
        function sortFilterInput() {
            var container = document.getElementById('sort-filter-form-container');
                
            var sortSelect = document.createElement('select');
            sortSelect.name = 'Sort_by';

            var nameOption = document.createElement('option');
            nameOption.value = 'Name';
            nameOption.textContent = 'Sort by name';
            sortSelect.appendChild(nameOption);

            var spCountOption = document.createElement('option');
            spCountOption.value = 'Sp_count';
            spCountOption.textContent = 'Sort by specialization count';
            sortSelect.appendChild(spCountOption);

            var noSortOption = document.createElement('option');
            noSortOption.value = '';
            noSortOption.textContent = 'No sort';
            noSortOption.selected = 'true';
            sortSelect.appendChild(noSortOption);

            var inLastName = document.createElement('input');
            inLastName.placeholder = 'In last name';
            var inFirstName = document.createElement('input');
            inFirstName.placeholder = 'In first name';
            var inMiddleName = document.createElement('input');
            inMiddleName.placeholder = 'In middle name';

            var setButton = document.createElement('button');
            setButton.textContent = 'Set';
            setButton.onclick = function() {
                var form = document.createElement('form');
                form.method = 'get';
                form.action = 'people.php';

                var sortInput = document.createElement('input');
                sortInput.type = 'hidden';
                sortInput.name = 'Sort_by';
                sortInput.value = sortSelect.value;

                var inLastNameInput = document.createElement('input');
                inLastNameInput.type = 'hidden';
                inLastNameInput.name = 'inLastName';
                inLastNameInput.value = inLastName.value;

                var inFirstNameInput = document.createElement('input');
                inFirstNameInput.type = 'hidden';
                inFirstNameInput.name = 'inFirstName';
                inFirstNameInput.value = inFirstName.value;

                var inMiddleNameInput = document.createElement('input');
                inMiddleNameInput.type = 'hidden';
                inMiddleNameInput.name = 'inMiddleName';
                inMiddleNameInput.value = inMiddleName.value;

                form.appendChild(sortInput);
                form.appendChild(inLastNameInput);
                form.appendChild(inFirstNameInput);
                form.appendChild(inMiddleNameInput);
                document.body.appendChild(form);
                form.submit();
            };

            var sortText = document.createElement('p');
            sortText.textContent = 'Sort:';
            var filtersText = document.createElement('p');
            filtersText.textContent = 'Filters:';

            container.appendChild(sortText);
            container.appendChild(sortSelect);
            container.appendChild(filtersText);
            container.appendChild(inLastName);
            container.appendChild(inFirstName);
            container.appendChild(inMiddleName);
            container.appendChild(setButton);
        }
    </script>
    <?php
        if ($_SERVER['REQUEST_METHOD'] == 'POST') {
            $method = $_POST['Method'];

            if ($method == 'delete') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $personID = $_POST['Person_id'];

                    $pdo->beginTransaction();

                    try {
                        // if a specialization does not belong to any company or person delete it (get this specializations)
                        $sql1 = "SELECT Sp_id FROM Specializations_of_people WHERE Person_id = :personID";
                        $stmt1 = $pdo->prepare($sql1);
                        $stmt1->bindParam(':personID', $personID, PDO::PARAM_INT);
                        $stmt1->execute();
                        $spIds = $stmt1->fetchAll(PDO::FETCH_COLUMN);

                        if (!empty($spIds)) {
                            $sql2 = "SELECT Sp_id, COUNT(*) as count_c 
                                    FROM Specializations_of_companies
                                    WHERE Sp_id IN (" . implode(',', array_fill(0, count($spIds), '?')) . ")
                                    GROUP BY Sp_id";
                            $stmt2 = $pdo->prepare($sql2);
                            $stmt2->execute($spIds);
                            $countForCompanies = $stmt2->fetchAll(PDO::FETCH_ASSOC);

                            $sql3 = "SELECT Sp_id, COUNT(*) as count_p 
                                    FROM Specializations_of_people
                                    WHERE Sp_id IN (" . implode(',', array_fill(0, count($spIds), '?')) . ")
                                    GROUP BY Sp_id";
                            $stmt3 = $pdo->prepare($sql3);
                            $stmt3->execute($spIds);
                            $countForPeople = $stmt3->fetchAll(PDO::FETCH_ASSOC);

                            $result = [];
                            foreach ($countForCompanies as $row) {
                                $result[$row['Sp_id']] = ['count_c' => $row['count_c'], 'count_p' => 0];
                            }
                            foreach ($countForPeople as $row) {
                                if (isset($result[$row['Sp_id']])) {
                                    $result[$row['Sp_id']]['count_p'] = $row['count_p'];
                                } else {
                                    $result[$row['Sp_id']] = ['count_c' => 0, 'count_p' => $row['count_p']];
                                }
                            }
                        }

                        //delete a person from specializations_of_people
                        $sql = "DELETE FROM Specializations_of_people WHERE Person_id = :personID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                        $stmt->execute();

                        //delete a person from phones
                        $sql = "DELETE FROM Phones WHERE Person_id = :personID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                        $stmt->execute();

                        //delete a person from people
                        $sql = "DELETE FROM People WHERE Person_id = :personID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                        $stmt->execute();

                        // if a specialization does not belong to any company or person delete it (delete this specializations)
                        if (!empty($spIds)) {
                            foreach ($result as $spId => $counts) {
                                if (($counts['count_c'] + $counts['count_p']) == 1) {
                                    $sql = "DELETE FROM Specializations WHERE Sp_id = :spID";
                                    $stmt = $pdo->prepare($sql);
                                    $stmt->bindParam(':spID', $spId, PDO::PARAM_INT);
                                    $stmt->execute();
                                }
                            }
                        }

                        $pdo->commit();
                        echo "The person is successfully deleted";
                    } catch (PDOException $e) {
                        $pdo->rollBack();
                        echo "ERROR: " . $e->getMessage();
                    }

                    echo "<form id='redirectForm' action='../people.php' method='get'>
                            <button type='submit'>Return</button>
                        </form>";
                    echo "<script>
                            setTimeout(function() { document.getElementById('redirectForm').submit(); }, 20000);
                        </script>";
                } catch (PDOException $e) {
                    echo "ERROR: " . $e->getMessage();
                }
            } else if ($method == 'add') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
    
                    $companyID = $_POST['Company_id'];
                    $positionID = $_POST['Position_id'];
                    $lastName = $_POST['Last_name'];
                    $firstName = $_POST['First_name'];
                    $middleName = $_POST['Middle_name'];
                    $positionName = $_POST['Position_name'];
    
                    $pdo->beginTransaction();
    
                    try {
                        if ($positionID == -1) {
                            $sql = "INSERT INTO Positions (Name) VALUES (:positionName)";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':positionName', $positionName, PDO::PARAM_STR);
                            $stmt->execute();
    
                            $sql = "SELECT Position_id FROM Positions WHERE Name LIKE :positionName";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':positionName', $positionName, PDO::PARAM_STR);
                            $stmt->execute();
                            if ($stmt->rowCount() > 0) {
                                $positionID = $stmt->fetch(PDO::FETCH_ASSOC)["Position_id"];
                            } else {
                                $pdo->rollBack();
                                echo "ERROR: cannot add position";
                            }
                        }
    
                        $sql = "INSERT INTO People (Last_name, First_name, Middle_name, Company_id, Position_id) VALUES
                                (:lastName, :firstName, :middleName,
                                " . ($companyID == 'empty' ? 'NULL' : ':companyID') . ",
                                " . ($positionID == 'empty' ? 'NULL' : ':positionID') . "
                                )";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':lastName', $lastName, PDO::PARAM_STR);
                        $stmt->bindParam(':firstName', $firstName, PDO::PARAM_STR);
                        $stmt->bindParam(':middleName', $middleName, PDO::PARAM_STR);
                        if ($companyID != 'empty') {
                            $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                        }
                        if ($positionID != 'empty') {
                            $stmt->bindParam(':positionID', $positionID, PDO::PARAM_INT);
                        }
                        $stmt->execute();
    
                        $pdo->commit();
                        echo "The person is successfully added";
                    }
                    catch (PDOException $e) {
                        $pdo->rollBack();
                        echo "ERROR: " . $e->getMessage();
                    }
                    echo "<form id='redirectForm' action='../people.php' method='get'>
                            <button type='submit'>Return</button>
                        </form>";
                    echo "<script>
                            setTimeout(function() { document.getElementById('redirectForm').submit(); }, 20000);
                        </script>";
                } catch (PDOException $e) {
                    echo "ERROR: " . $e->getMessage();
                }
            }
        } else {
            echo "<div id='sort-filter-form-container'></div><br><br>";
            ?>
            <script>
                sortFilterInput();
            </script>
            <?php

            $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME , $DB_USER, $DB_PASSWORD);
            if (!$pdo) {
                echo 'error';
            } else {
                $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                $sortBy = isset($_GET['Sort_by']) ? $_GET['Sort_by'] : '';
                $orderBy = '';

                switch ($sortBy) {
                    case 'Name':
                        $orderBy = 'ORDER BY Last_name, First_name, Middle_name ASC';
                        break;
                    case 'Sp_count':
                        $orderBy = 'ORDER BY count DESC';
                        break;
                    default:
                        $orderBy = '';
                }

                
                $lastNameFilter = isset($_GET['inLastName']) ? $_GET['inLastName'] : '';
                $firstNameFilter = isset($_GET['inFirstName']) ? $_GET['inFirstName'] : '';
                $middleNameFilter = isset($_GET['inMiddleName']) ? $_GET['inMiddleName'] : '';

                $whereClause = [];
                if (!empty($lastNameFilter)) {
                    $whereClause[] = "Last_name LIKE '%$lastNameFilter%'";
                }
                if (!empty($firstNameFilter)) {
                    $whereClause[] = "First_name LIKE '%$firstNameFilter%'";
                }
                if (!empty($lastNameFilter)) {
                    $whereClause[] = "Middle_name LIKE '%$middleNameFilter%'";
                }

                $whereClause = !empty($whereClause) ? 'WHERE ' . implode(' AND ', $whereClause) : '';

                $sql = "WITH sp_count AS (
                            SELECT Person_id, Count(*) as count
                            FROM Specializations_of_people
                            GROUP BY Person_id
                        )
                        SELECT p.Person_id, Last_name, First_name, Middle_name, count
                        FROM People p
                        LEFT JOIN sp_count sp ON p.Person_id = sp.Person_id
                        $whereClause
                        $orderBy";
                $stmt = $pdo->prepare($sql);
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<p>People (and number of their specialties):</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        $count = $row['count'];
                        if ($count == NULL) {
                            $count = 0;
                        }
                        echo "<li>
                                <a href= \"person.php?Person_id=  ${row['Person_id']} \"> ${row['Last_name']}  ${row['First_name']}  ${row['Middle_name']} </a>
                                "  . str_repeat('&nbsp;', 5) . "$count
                                <form action='people.php' method='post' style='display:inline;'>
                                        <input type='hidden' name='Method' value='delete'>
                                        <input type='hidden' name='Person_id' value='${row['Person_id']}'>
                                        <button type='submit'>Delete</button>
                                </form>
                            </li>";
                    }
                    echo '</ol>';
                } else {
                    echo "table is empty";
                }
            };

            echo "<div id='new-person-container'>
                    <button id='add-person-btn' onclick='addPersonInput()'>Add Person</button>
                    <div id='person-input-container'></div>
                </div><br><br>";
        }
    ?><br>
    <script>
        function addPersonInput() {
            var container = document.getElementById('person-input-container');
                
            var lastName = document.createElement('input');
            lastName.placeholder = 'Enter last name';
            lastName.required = true;
            var firstName = document.createElement('input');
            firstName.placeholder = 'Enter first name';
            firstName.required = true;
            var middleName = document.createElement('input');
            middleName.placeholder = 'Enter middle name';
            middleName.required = true;
                
            var companySelect = document.createElement('select');
            companySelect.name = 'Company_id';
            var positionSelect = document.createElement('select');
            positionSelect.name = 'Position_id';

            var newOption = document.createElement('option');
            newOption.value = 'new';
            newOption.textContent = 'New position';
            positionSelect.appendChild(newOption);

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
                            companySelect.appendChild(option);
                            <?php
                        }
                    }

                    $sql = "SELECT Position_id, Name FROM Positions";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Position_id']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
                            positionSelect.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var emptyCompanyOption = document.createElement('option');
            emptyCompanyOption.value = 'empty';
            emptyCompanyOption.textContent = 'No company';
            companySelect.appendChild(emptyCompanyOption);

            var emptyPositionOption = document.createElement('option');
            emptyPositionOption.value = 'empty';
            emptyPositionOption.textContent = 'No position';
            positionSelect.appendChild(emptyPositionOption);

            var inputPositionName = document.createElement('input');
            inputPositionName.type = 'text';
            inputPositionName.placeholder = 'Enter new position name';
            inputPositionName.name = 'new_Position_name';

            positionSelect.onchange = function() {
                if (positionSelect.value === 'new') {
                    inputPositionName.style.display = 'block';
                } else {
                    inputPositionName.style.display = 'none';
                }
            };

            var addButton = document.createElement('button');
            addButton.textContent = 'Save';
            addButton.onclick = function() {
                if (positionSelect.value === 'new' && inputPositionName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a new position name');
                    return false;
                } else if (positionSelect.value === 'new' && inputPositionName.value.length > 255) {
                    event.preventDefault();
                    alert('Position name must be <= 255 symbols');
                    return false;
                } else if (lastName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a last name');
                    return false;
                } else if (firstName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a first name');
                    return false;
                } else if (middleName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a middle name');
                    return false;
                }

                var positionName = positionSelect.value === 'new' ? inputPositionName.value : '';
                var positionID = positionSelect.value === 'new' ? -1 : positionSelect.value;

                var form = document.createElement('form');
                form.method = 'post';
                form.action = 'people.php';

                var methodInput = document.createElement('input');
                methodInput.type = 'hidden';
                methodInput.name = 'Method';
                methodInput.value = 'add';

                var positionNameInput = document.createElement('input');
                positionNameInput.type = 'hidden';
                positionNameInput.name = 'Position_name';
                positionNameInput.value = positionName;

                var positionIDInput = document.createElement('input');
                positionIDInput.type = 'hidden';
                positionIDInput.name = 'Position_id';
                positionIDInput.value = positionID;

                var lastNameInput = document.createElement('input');
                lastNameInput.type = 'hidden';
                lastNameInput.name = 'Last_name';
                lastNameInput.value = lastName.value;

                var firstNameInput = document.createElement('input');
                firstNameInput.type = 'hidden';
                firstNameInput.name = 'First_name';
                firstNameInput.value = firstName.value;

                var middleNameInput = document.createElement('input');
                middleNameInput.type = 'hidden';
                middleNameInput.name = 'Middle_name';
                middleNameInput.value = middleName.value;

                var companyIDInput = document.createElement('input');
                companyIDInput.type = 'hidden';
                companyIDInput.name = 'Company_id';
                companyIDInput.value = companySelect.value;

                form.appendChild(methodInput);
                form.appendChild(companyIDInput);
                form.appendChild(positionIDInput);
                form.appendChild(lastNameInput);
                form.appendChild(firstNameInput);
                form.appendChild(middleNameInput);
                form.appendChild(positionNameInput);
                document.body.appendChild(form);
                form.submit();
            };

            container.appendChild(lastName);
            container.appendChild(firstName);
            container.appendChild(middleName);
            container.appendChild(companySelect);
            container.appendChild(positionSelect);
            container.appendChild(inputPositionName);
            container.appendChild(addButton);

            document.getElementById('add-person-btn').style.display = 'none';
        }
    </script>
</body>
</html>