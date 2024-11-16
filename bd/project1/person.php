<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Person</title>
<!-- ФИО, номера телефонов, компания и должность если есть
специализации списком
+ редактирование фио, компании, должности
+ добавление / удаление номеров телефона
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

            if ($method == 'deleteSp') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $spID = $_POST['Sp_id'];
                    $personID = $_POST['Person_id'];

                    $pdo->beginTransaction();

                    try {
                        //delete a specialisation for this person
                        $sql = "DELETE FROM Specializations_of_people WHERE Sp_id = :spID AND Person_id = :personID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                        $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
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
            } else if ($method == 'deletePhone') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $phoneID = $_POST['Phone_id'];
                    $personID = $_POST['Person_id'];

                    $pdo->beginTransaction();

                    try {
                        $sql = "DELETE FROM Phones WHERE Phone_id = :phoneID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':phoneID', $phoneID, PDO::PARAM_INT);
                        $stmt->execute();

                        $pdo->commit();
                        echo "The phone is successfully deleted";
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
            } else if ($method == 'addSp') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $spName = $_POST['Sp_name'];
                    $personID = $_POST['Person_id'];

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
                            $sql = "INSERT INTO Specializations_of_people (Sp_id, Person_id) VALUES (:spID, :personID)";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':spID', $spID, PDO::PARAM_INT);
                            $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                            $stmt->execute();
                        }

                        $pdo->commit();
                        echo "The specialization is successfully added";
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
            } else if ($method == 'addPhone') {
                try {
                    $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $cityID = $_POST['City_id'];
                    $cityName = $_POST['City_name'];
                    $postCode = $_POST['Post_code'];
                    $phoneCode = $_POST['Phone_code'];
                    $phoneNumber = $_POST['Phone_number'];
                    $personID = $_POST['Person_id'];

                    $pdo->beginTransaction();

                    try {
                        if ($cityID == 0) {
                            $sql = "INSERT INTO Cities (Name, Post_code, Phone_code) VALUES (:cityName, :postCode, :phoneCode)";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':cityName', $cityName, PDO::PARAM_STR);
                            $stmt->bindParam(':postCode', $postCode, PDO::PARAM_INT);
                            $stmt->bindParam(':phoneCode', $phoneCode, PDO::PARAM_INT);
                            $stmt->execute();

                            $sql = "SELECT City_id FROM Cities WHERE Name LIKE :cityName";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':cityName', $cityName, PDO::PARAM_STR);
                            $stmt->execute();
                            if ($stmt->rowCount() > 0) {
                                $cityID = $stmt->fetch(PDO::FETCH_ASSOC)['City_id'];
                            } else {
                                $pdo->rollBack();
                                echo "ERROR: cannot add city";
                            }
                        }

                        $sql = "SELECT COUNT(*) as count FROM Phones
                                WHERE (Phone_number LIKE :phoneNumber) AND (City_id LIKE :cityID) AND (Person_id LIKE :personID)";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':phoneNumber', $phoneNumber, PDO::PARAM_STR);
                        $stmt->bindParam(':cityID', $cityID, PDO::PARAM_INT);
                        $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                        $stmt->execute();
                        if ($stmt->rowCount() > 0) { 
                            $count = $stmt->fetch(PDO::FETCH_ASSOC)['count'];
                            if ($count == 0) {
                                $sql = "INSERT INTO Phones (Phone_number, City_id, Person_id) VALUES (:phoneNumber, :cityID, :personID)";
                                $stmt = $pdo->prepare($sql);
                                $stmt->bindParam(':phoneNumber', $phoneNumber, PDO::PARAM_STR);
                                $stmt->bindParam(':cityID', $cityID, PDO::PARAM_INT);
                                $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                                $stmt->execute();
                            }
                        }

                        $pdo->commit();
                        echo "The phone is successfully added";
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
            }
        } else {
            $personID = $_GET['Person_id'];

            echo "<p>Person</p><br>";

            $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
            if (!$pdo) {
                echo 'error';
            } else {
                $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                $sql = "SELECT p.Person_id, p.Last_name, p.First_name, p.Middle_name, p.Company_id, p.Position_id,
                                comp.Short_name,
                                pos.Name
                    FROM People as p
                    LEFT JOIN Companies as comp USING (Company_id)
                    LEFT JOIN Positions as pos USING (Position_id)
                    WHERE Person_id = :personID";
                $stmt = $pdo->prepare($sql);
                $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<table border='1'>
                            <tr>
                                <th>Last_name</th>
                                <th>First_name</th>
                                <th>Middle_name</th>
                                <th>Company</th>
                                <th>Position</th>
                            </tr>";
        
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        if ($row["Company_id"] === NULL) {
                            echo "<tr>
                                <td>" . $row["Last_name"] . "</td>
                                <td>" . $row["First_name"] . "</td>
                                <td>" . $row["Middle_name"] . "</td>
                                <td> no company </td>
                                <td>" . ($row["Position_id"] === NULL ? "no position" : $row["Name"]) . "</td>
                            </tr>";
                        } else {
                            echo "<tr>
                                    <td>" . $row["Last_name"] . "</td>
                                    <td>" . $row["First_name"] . "</td>
                                    <td>" . $row["Middle_name"] . "</td>
                                    <td><a href=\"company.php?Company_id=${row['Company_id']}\"> ${row['Short_name']}</a></td>
                                    <td>" . ($row["Position_id"] === NULL ? "no position" : $row["Name"]) . "</td>
                                </tr>";
                        }
                    }
                    echo "</table>";
                } else {
                    echo "table is empty";
                }

                echo "<br><form action='../edit/edit_person.php' method='get' style='display:inline;'>
                            <input type='hidden' name='Person_id' value='" . $personID . "'>
                            <button type='submit'>Edit</button>
                        </form>";
                
                $sql = "SELECT Phone_id, Phone_number, Phone_code
                        FROM Phones
                        INNER JOIN Cities USING (City_id)
                        WHERE Person_id = :personID";
                $stmt = $pdo->prepare($sql);
                $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<p>Phones:</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<li>
                                <td>(" . $row["Phone_code"] . ")" . $row["Phone_number"] . "</td>
                                <form action='person.php' method='post' style='display:inline;'>
                                    <input type='hidden' name='Method' value='deletePhone'>
                                    <input type='hidden' name='Phone_id' value='" . $row['Phone_id'] . "'>
                                    <input type='hidden' name='Person_id' value='" . $personID . "'>
                                    <button type='submit'>Delete</button>
                                </form>
                            </li>";
                    }
                    echo '</ol>';
                } else {
                    echo "table is empty";
                }

                echo "<div id='new-phone-container'>
                        <button id='add-phone-btn' onclick='addPhoneInput()'>Add Phone</button>
                        <div id='phone-input-container'></div>
                    </div><br><br>";

                $sql = "SELECT sc.Person_id, sc.Sp_id, s.Name
                        FROM Specializations_of_people as sc
                        INNER JOIN Specializations as s USING (Sp_id)
                        WHERE sc.Person_id = :personID";
                $stmt = $pdo->prepare($sql);
                $stmt->bindParam(':personID', $personID, PDO::PARAM_INT);
                $stmt->execute();

                if ($stmt->rowCount() > 0) {
                    echo "<p>Specializations:</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        echo "<li>
                                <td>" . $row["Name"] . "</td>
                                <form action='person.php' method='post' style='display:inline;'>
                                    <input type='hidden' name='Method' value='deleteSp'>
                                    <input type='hidden' name='Sp_id' value='" . $row['Sp_id'] . "'>
                                    <input type='hidden' name='Person_id' value='" . $row['Person_id'] . "'>
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
                var form = document.createElement('form');
                form.method = 'post';
                form.action = '../person.php';

                var methodInput = document.createElement('input');
                methodInput.type = 'hidden';
                methodInput.name = 'Method';
                methodInput.value = 'addSp';

                var personIDInput = document.createElement('input');
                personIDInput.type = 'hidden';
                personIDInput.name = 'Person_id';
                personIDInput.value = '<?php echo $personID; ?>';

                var spNameInput = document.createElement('input');
                spNameInput.type = 'hidden';
                spNameInput.name = 'Sp_name';
                spNameInput.value = spName;

                form.appendChild(methodInput);
                form.appendChild(personIDInput);
                form.appendChild(spNameInput);
                document.body.appendChild(form);
                form.submit();
            };

            container.appendChild(select);
            container.appendChild(input);
            container.appendChild(addButton);

            document.getElementById('add-specialization-btn').style.display = 'none';
        }

        function addPhoneInput() {
            var container = document.getElementById('phone-input-container');
            
            var select = document.createElement('select');
            select.name = 'City';

            var newOption = document.createElement('option');
            newOption.value = 'new';
            newOption.textContent = 'New city';
            select.appendChild(newOption);

            <?php
                $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                if (!$pdo) {
                    echo 'error';
                } else {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $sql = "SELECT Name, City_id
                            FROM Cities";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                    
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['City_id']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
                            select.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var inputCityName = document.createElement('input');
            inputCityName.type = 'text';
            inputCityName.placeholder = 'Enter new city name';
            inputCityName.name = 'new_City_name';

            var inputPostCode = document.createElement('input');
            inputPostCode.type = 'int';
            inputPostCode.placeholder = 'Enter city post code';
            inputPostCode.name = 'new_Post_code';

            var inputPhoneCode = document.createElement('input');
            inputPhoneCode.type = 'int';
            inputPhoneCode.placeholder = 'Enter city phone code';
            inputPhoneCode.name = 'new_Phone_code';

            var inputPhoneNumber = document.createElement('input');
            inputPhoneNumber.type = 'text';
            inputPhoneNumber.placeholder = 'Enter phone number';

            select.onchange = function() {
                if (select.value === 'new') {
                    inputCityName.style.display = 'block';
                    inputPostCode.style.display = 'block';
                    inputPhoneCode.style.display = 'block';
                } else {
                    inputCityName.style.display = 'none';
                    inputPostCode.style.display = 'none';
                    inputPhoneCode.style.display = 'none';
                }
            };

            var addButton = document.createElement('button');
            addButton.textContent = 'Save';
            addButton.onclick = function() {
                if (select.value === 'new' && inputCityName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a new city name');
                    return false;
                } else if (select.value === 'new' && inputCityName.value.length > 255) {
                    event.preventDefault();
                    alert('City name must be <= 255 symbols');
                    return false;
                } else if (select.value === 'new' && inputPostCode.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a city post code');
                    return false;
                } else if (select.value === 'new' && inputPhoneCode.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a city phone code');
                    return false;
                } else if (select.value === 'new' && (isNaN(inputPostCode.value) || inputPostCode.value < 100 || inputPostCode.value > 999)) {
                    event.preventDefault();
                    alert('Post code must be a number between 100 and 999');
                    return false;
                } else if (select.value === 'new' && (isNaN(inputPhoneCode.value) || inputPhoneCode.value > 999 || inputPhoneCode.value < 100)) {
                    event.preventDefault();
                    alert('Phone code must be a number between 100 and 999');
                    return false;
                } else if (inputPhoneNumber.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a phone number');
                    return false;
                } else if (inputPhoneNumber.value.length != 7 || !/^\d+$/.test(inputPhoneNumber.value)) {
                    event.preventDefault();
                    alert('Phone number must have 7 numbers and contain only digits. Your length is ' + inputPhoneNumber.value.length);
                    return false;
                }

                var cityName = select.value === 'new' ? inputCityName.value : '';
                var cityID = select.value === 'new' ? 0 : select.value;
                var postCode = select.value === 'new' ? inputPostCode.value : 0;
                var phoneCode = select.value === 'new' ? inputPhoneCode.value : 0;
                
                var form = document.createElement('form');
                form.method = 'post';
                form.action = '../person.php';

                var methodInput = document.createElement('input');
                methodInput.type = 'hidden';
                methodInput.name = 'Method';
                methodInput.value = 'addPhone';

                var personIDInput = document.createElement('input');
                personIDInput.type = 'hidden';
                personIDInput.name = 'Person_id';
                personIDInput.value = '<?php echo $personID; ?>';

                var cityIDInput = document.createElement('input');
                cityIDInput.type = 'hidden';
                cityIDInput.name = 'City_id';
                cityIDInput.value = cityID;

                var cityNameInput = document.createElement('input');
                cityNameInput.type = 'hidden';
                cityNameInput.name = 'City_name';
                cityNameInput.value = cityName;

                var postCodeInput = document.createElement('input');
                postCodeInput.type = 'hidden';
                postCodeInput.name = 'Post_code';
                postCodeInput.value = postCode;
                
                var phoneCodeInput = document.createElement('input');
                phoneCodeInput.type = 'hidden';
                phoneCodeInput.name = 'Phone_code';
                phoneCodeInput.value = phoneCode;

                var phoneNumberInput = document.createElement('input');
                phoneNumberInput.type = 'hidden';
                phoneNumberInput.name = 'Phone_number';
                phoneNumberInput.value = inputPhoneNumber.value;

                form.appendChild(methodInput);
                form.appendChild(personIDInput);
                form.appendChild(cityIDInput);
                form.appendChild(cityNameInput);
                form.appendChild(postCodeInput);
                form.appendChild(phoneCodeInput);
                form.appendChild(phoneNumberInput);
                document.body.appendChild(form);
                form.submit();
            };

            container.appendChild(select);
            container.appendChild(inputCityName);
            container.appendChild(inputPostCode);
            container.appendChild(inputPhoneCode);
            container.appendChild(inputPhoneNumber);
            container.appendChild(addButton);

            document.getElementById('add-phone-btn').style.display = 'none';
        }
    </script>
</body>
</html>