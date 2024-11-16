<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Company</title>
<!-- Названия, адрес полный
специализации списком
люди в компании (фио ссылки)
+ редактирование адреса и названия
+ добавление / удаление специализаций -->
<body>
    <a href="../index.php"> Home page </a><br>

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
    
    <?php
        if ($_SERVER['REQUEST_METHOD'] == 'POST') {
            $method = $_POST['Method'];

            if ($method == 'delete') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $spID = $_POST['Sp_id'];
                    $companyID = $_POST['Company_id'];

                    $pdo->beginTransaction();

                    try {
                        //delete a specialisation for this company
                        $sql = "DELETE FROM Specializations_of_companies WHERE Sp_id = :spID AND Company_id = :companyID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                        $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                        $stmt->execute();

                        // if a specialization does not belong to any company or person delete it
                        $sql1 = "SELECT COUNT(*) as count FROM Specializations_of_companies WHERE Sp_id = :spID";
                        $stmt1 = $pdo->prepare($sql1);
                        $stmt1->bindParam(':spID', $spID, PDO::PARAM_INT);
                        $stmt1->execute();
                        if ($stmt1->rowCount() > 0) { 
                            if ($stmt1->fetch(PDO::FETCH_ASSOC)["count"] == 0) {
                                $sql2 = "SELECT COUNT(*) as count FROM Specializations_of_people WHERE Sp_id = :spID";
                                $stmt2 = $pdo->prepare($sql2);
                                $stmt2->bindParam(':spID', $spID, PDO::PARAM_INT);
                                $stmt2->execute();
                                if ($stmt2->rowCount() > 0) { 
                                    if ($stmt2->fetch(PDO::FETCH_ASSOC)["count"] == 0) {
                                        $sql = "DELETE FROM Specializations WHERE Sp_id = :spID";
                                        $stmt = $pdo->prepare($sql);
                                        $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                                        $stmt->execute();
                                    }
                                }
                            }
                        }

                        $pdo->commit();
                        echo "The specialization is successfully deleted";
                    }
                    catch (PDOException $e) {
                        $pdo->rollBack();
                        echo "ERROR: " . $e->getMessage();
                    }
                    echo "<form id='redirectForm' action='../company.php' method='get'>
                            <input type='hidden' name='Company_id' value='" . $companyID . "'>
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

                    $spName = $_POST['Sp_name'];
                    $companyID = $_POST['Company_id'];

                    $pdo->beginTransaction();

                    try {
                        $sql = "SELECT Count(*) as count FROM Specializations WHERE Name LIKE :spName";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':spName', $spName, PDO::PARAM_STR);
                        $stmt->execute();
                        if ($stmt->rowCount() > 0) { 
                            $count = $stmt->fetch(PDO::FETCH_ASSOC)['count'];
                            if ($count == 0) {
                                $sql = "INSERT INTO Specializations (Name) VALUES (:spName)";
                                $stmt = $pdo->prepare($sql);
                                $stmt->bindParam(':spName', $spName, PDO::PARAM_STR);
                                $stmt->execute();
                            }
                        }

                        $sql = "SELECT Sp_id FROM Specializations WHERE Name LIKE :spName";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':spName', $spName, PDO::PARAM_STR);
                        $stmt->execute();
                        if ($stmt->rowCount() > 0) { 
                            $spID = $stmt->fetch(PDO::FETCH_ASSOC)['Sp_id'];
                            $sql = "INSERT INTO Specializations_of_companies (Sp_id, Company_id) VALUES (:spID, :companyID)";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                            $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                            $stmt->execute();
                        }

                        $pdo->commit();
                        echo "The specialization is successfully added";
                    }
                    catch (PDOException $e) {
                        $pdo->rollBack();
                        echo "ERROR: " . $e->getMessage();
                    }
                    echo "<form id='redirectForm' action='../company.php' method='get'>
                            <input type='hidden' name='Company_id' value='" . $companyID . "'>
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
            $companyID = $_GET['Company_id'];

            echo "<p>Company</p><br>";

            $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
            if (!$pdo) {
                echo 'error';
            } else {
                $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                $sql = "SELECT comp.Company_id, comp.Short_name, comp.Full_name, 
                                addr.Street, addr.House, addr.Post_office_code,
                                c.Name, c.Post_code
                    FROM Companies as comp
                    INNER JOIN Addresses as addr USING (Address_id)
                    INNER JOIN Cities as c USING (City_id)
                    WHERE Company_id = :companyID";
                $stmt = $pdo->prepare($sql);
                $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<table border='1'>
                            <tr>
                                <th>Short_name</th>
                                <th>Full_name</th>
                                <th>City</th>
                                <th>Street</th>
                                <th>House</th>
                                <th>Post_code</th>
                            </tr>";
        
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<tr>
                                <td>" . $row["Short_name"] . "</td>
                                <td>" . $row["Full_name"] . "</td>
                                <td>" . $row["Name"] . "</td>
                                <td>" . $row["Street"] . "</td>
                                <td>" . $row["House"] . "</td>
                                <td>" . $row["Post_code"] . $row["Post_office_code"] . "</td>
                            </tr>";
                    }
                    echo "</table>";
                } else {
                    echo "table is empty";
                }

                echo "<br><form action='../edit/edit_company.php' method='get' style='display:inline;'>
                            <input type='hidden' name='Company_id' value='" . $companyID . "'>
                            <button type='submit'>Edit</button>
                        </form>";
                
                $sql = "SELECT sc.Company_id, s.Name, s.Sp_id
                        FROM Specializations_of_companies as sc
                        INNER JOIN Specializations as s USING (Sp_id)
                        WHERE sc.Company_id = :companyID";
                $stmt = $pdo->prepare($sql);
                $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                $stmt->execute();

                if ($stmt->rowCount() > 0) {
                    echo "<p>Specializations:</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<li>
                                <td>" . $row["Name"] . "</td>
                                <form action='company.php' method='post' style='display:inline;'>
                                    <input type='hidden' name='Method' value='delete'>
                                    <input type='hidden' name='Sp_id' value='" . $row['Sp_id'] . "'>
                                    <input type='hidden' name='Company_id' value='" . $row['Company_id'] . "'>
                                    <button type='submit'>Delete</button>
                                </form>
                            </li>";
                    }
                    echo '</ol>';
                } else {
                    echo "table is empty";
                }
                
                echo "<div id='new-specialization-container'>
                        <button id='add-specialization-btn' onclick='addSpecializationInput()'>Add Specialization</button>
                        <div id='specialization-input-container'></div>
                    </div><br><br>";

                $sql = "SELECT Person_id, Last_name, First_name, Middle_name, Positions.Name
                        FROM People
                        INNER JOIN Positions USING (Position_id)
                        WHERE Company_id = :companyID";
                $stmt = $pdo->prepare($sql);
                $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<table border='1'>
                            <tr>
                                <td>Employee</td>
                                <td>Position</td>
                            </tr>";
        
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<tr>
                                <td><a href=\"person.php?Person_id=${row['Person_id']}\"> ${row['Last_name']}  ${row['First_name']}  ${row['Middle_name']} </a></td>
                                <td>" . $row["Name"] . "</td>
                            </tr>";
                    }
                    echo "</table>";
                } else {
                    echo "table is empty";
                }
            }
        }
    ?><br>
    <script>
        function addSpecializationInput() {
            var container = document.getElementById('specialization-input-container');
            
            var select = document.createElement('select');
            select.name = 'Sp_name';

            var newOption = document.createElement('option');
            newOption.value = 'new';
            newOption.textContent = 'New Specialization';
            select.appendChild(newOption);

            <?php
                $DB_HOST = 'localhost';
                $DB_NAME = 'katerina';
                $DB_USER = 'root';
                $DB_PASSWORD = 'root';
                $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                if (!$pdo) {
                    echo 'error';
                } else {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $sql = "SELECT Sp_id, Name
                        FROM Specializations as sp";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                    
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Name']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
                            select.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var input = document.createElement('input');
            input.type = 'text';
            input.placeholder = 'Enter new specialization';
            input.name = 'new_Sp_name';

            select.onchange = function() {
                if (select.value === 'new') {
                    input.style.display = 'block';
                } else {
                    input.style.display = 'none';
                }
            };

            var addButton = document.createElement('button');
            addButton.textContent = '+';
            addButton.onclick = function() {
                if (select.value === 'new' && input.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a new specialization name');
                    return false;
                } else if (select.value === 'new' && input.value.length > 255) {
                    event.preventDefault();
                    alert('Specialization name must be <= 255 symbols');
                    return false;
                }
                
                var spName = select.value === 'new' ? input.value : select.value;
                if (spName) {
                    var form = document.createElement('form');
                    form.method = 'post';
                    form.action = '../company.php';

                    var methodInput = document.createElement('input');
                    methodInput.type = 'hidden';
                    methodInput.name = 'Method';
                    methodInput.value = 'add';

                    var companyIDInput = document.createElement('input');
                    companyIDInput.type = 'hidden';
                    companyIDInput.name = 'Company_id';
                    companyIDInput.value = '<?php echo $companyID; ?>';

                    var spNameInput = document.createElement('input');
                    spNameInput.type = 'hidden';
                    spNameInput.name = 'Sp_name';
                    spNameInput.value = spName;

                    form.appendChild(methodInput);
                    form.appendChild(companyIDInput);
                    form.appendChild(spNameInput);
                    document.body.appendChild(form);
                    form.submit();
                }
            };

            container.appendChild(select);
            container.appendChild(input);
            container.appendChild(addButton);

            document.getElementById('add-specialization-btn').style.display = 'none';
        }
    </script>
</body>
</html>