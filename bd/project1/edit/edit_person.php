<?php
    session_start();
?>
<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Edit person</title>
<body>
    <?php
        $personID = $_GET['Person_id'];
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
        function editPersonInput() {
            var container = document.getElementById('editform-container');
                
            var lastName = document.createElement('input');
            lastName.required = true;
            var firstName = document.createElement('input');
            firstName.required = true;
            var middleName = document.createElement('input');
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

                    $sql = "SELECT Last_name, First_name, Middle_name, Company_id, Position_id
                            FROM People
                            WHERE Person_id = :personID";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                    $stmt->execute();

                    if ($stmt->rowCount() > 0) {
                        $company = $stmt->fetch(PDO::FETCH_ASSOC);
                        ?>
                        lastName.type = 'text';
                        lastName.value = '<?php echo $company['Last_name']; ?>';
                        lastName.placeholder = '<?php echo $company['Last_name']; ?>';

                        firstName.type = 'text';
                        firstName.value = '<?php echo $company['First_name']; ?>';
                        firstName.placeholder = '<?php echo $company['First_name']; ?>';

                        middleName.type = 'text';
                        middleName.value = '<?php echo $company['Middle_name']; ?>';
                        middleName.placeholder = '<?php echo $company['Middle_name']; ?>';

                        <?php
                            $currentCompanyID = $company['Company_id'];
                            $currentPositionID = $company['Position_id'];
                    }

                    $sql = "SELECT Company_id, Short_name FROM Companies";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            $selected = ($row['Company_id'] == $currentCompanyID) ? 'selected' : '';
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Company_id']; ?>';
                            option.textContent = '<?php echo $row['Short_name']; ?>';
                            option.selected = <?php echo $selected ? 'true' : 'false'; ?>;
                            companySelect.appendChild(option);
                            <?php
                        }
                    }

                    $sql = "SELECT Position_id, Name FROM Positions";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            $selected = ($row['Position_id'] == $currentPositionID) ? 'selected' : '';
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['Position_id']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
                            option.selected = <?php echo $selected ? 'true' : 'false'; ?>;
                            positionSelect.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var emptyCompanyOption = document.createElement('option');
            emptyCompanyOption.value = 'empty';
            emptyCompanyOption.textContent = 'No company';
            emptyCompanyOption.selected = <?php echo ($currentCompanyID === NULL) ? 'true' : 'false'; ?>;
            companySelect.appendChild(emptyCompanyOption);

            var emptyPositionOption = document.createElement('option');
            emptyPositionOption.value = 'empty';
            emptyPositionOption.textContent = 'No position';
            emptyPositionOption.selected = <?php echo ($currentPositionID === NULL) ? 'true' : 'false'; ?>;
            positionSelect.appendChild(emptyPositionOption);

            var inputPositionName = document.createElement('input');
            inputPositionName.type = 'text';
            inputPositionName.placeholder = 'Enter new position name';
            inputPositionName.name = 'new_Position_name';
            inputPositionName.style.display = 'none';

            positionSelect.onchange = function() {
                if (positionSelect.value === 'new') {
                    inputPositionName.style.display = 'block';
                } else {
                    inputPositionName.style.display = 'none';
                }
            };

            var editButton = document.createElement('button');
            editButton.textContent = 'Save';
            editButton.onclick = function() {
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
                form.action = 'edit_person.php';

                var personIDInput = document.createElement('input');
                personIDInput.type = 'hidden';
                personIDInput.name = 'Person_id';
                personIDInput.value = '<?php echo $personID; ?>';

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

                form.appendChild(companyIDInput);
                form.appendChild(positionIDInput);
                form.appendChild(lastNameInput);
                form.appendChild(firstNameInput);
                form.appendChild(middleNameInput);
                form.appendChild(positionNameInput);
                form.appendChild(personIDInput);
                document.body.appendChild(form);
                form.submit();
            };

            container.appendChild(lastName);
            container.appendChild(firstName);
            container.appendChild(middleName);
            container.appendChild(companySelect);
            container.appendChild(positionSelect);
            container.appendChild(inputPositionName);
            container.appendChild(editButton);
        }
    </script>
    <?php
        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
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
                $personID = $_POST['Person_id'];

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

                    $sql = "UPDATE People SET
                            Last_name = :lastName, 
                            First_name = :firstName, 
                            Middle_name = :middleName,
                            Company_id = " . ($companyID == 'empty' ? 'NULL' : ':companyID') . ",
                            Position_id = " . ($positionID == 'empty' ? 'NULL' : ':positionID') . "
                            WHERE Person_id = :personID";
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
                    $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                    $stmt->execute();

                    $pdo->commit();
                    echo "The person is successfully updated";
                }
                catch (PDOException $e) {
                    $pdo->rollBack();
                    echo "ERROR: " . $e->getMessage();
                }
                echo "<form id='redirectForm' action='../person.php' method='get'>
                        <input type='hidden' name='Person_id' value='" . $personID . "'>
                        <button type='submit'>Return</button>
                    </form>";
                echo "<script>
                        setTimeout(function() { document.getElementById('redirectForm').submit(); }, 20000);
                    </script>";
            } catch (PDOException $e) {
                echo "ERROR: " . $e->getMessage();
            }
        } else {
            echo "EDIT FORM<br>";
            echo "<div id='editform-container'></div><br><br>";
            ?>
            <script>
                editPersonInput();
            </script>
            <?php
                echo "<form id='redirectForm' action='../person.php' method='get'>
                            <input type='hidden' name='Person_id' value='" . $personID . "'>
                            <button type='submit'>Return</button>
                        </form>";
        }
    ?>
</body>
</html>