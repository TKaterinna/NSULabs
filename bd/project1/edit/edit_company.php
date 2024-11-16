<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Edit company</title>
<body>
    <?php
        $companyID = $_GET['Company_id'];
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
        function editCompanyInput() {
            var container = document.getElementById('editform-container');
                
            var shortName = document.createElement('input');
            shortName.required = true;
            var fullName = document.createElement('input');
            fullName.required = true;
            var street = document.createElement('input');
            street.required = true;
            var house = document.createElement('input');
            house.required = true;
            var postOfficeCode = document.createElement('input');
            postOfficeCode.required = true;
                
            var citySelect = document.createElement('select');
            citySelect.name = 'City';

            var newOption = document.createElement('option');
            newOption.value = 'new';
            newOption.textContent = 'New city';
            citySelect.appendChild(newOption);

            <?php
                $pdo = new PDO("mysql:host=" . $DB_HOST . "; dbname=" . $DB_NAME, $DB_USER, $DB_PASSWORD);
                if (!$pdo) {
                    echo 'error';
                } else {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
                    $pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

                    $sql = "SELECT comp.Company_id, comp.Short_name, comp.Full_name, 
                                    addr.Street, addr.House, addr.Post_office_code,
                                    c.Name, c.City_id, c.Post_code
                            FROM Companies as comp
                            INNER JOIN Addresses as addr USING (Address_id)
                            INNER JOIN Cities as c USING (City_id)
                            WHERE Company_id = :companyID";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                    $stmt->execute();

                    if ($stmt->rowCount() > 0) {
                        $company = $stmt->fetch(PDO::FETCH_ASSOC);
                        ?>
                        shortName.type = 'text';
                        shortName.value = '<?php echo $company['Short_name']; ?>';
                        shortName.placeholder = '<?php echo $company['Short_name']; ?>';

                        fullName.type = 'text';
                        fullName.value = '<?php echo $company['Full_name']; ?>';
                        fullName.placeholder = '<?php echo $company['Full_name']; ?>';

                        street.type = 'text';
                        street.value = '<?php echo $company['Street']; ?>';
                        street.placeholder = '<?php echo $company['Street']; ?>';

                        house.type = 'text';
                        house.value = '<?php echo $company['House']; ?>';
                        house.placeholder = '<?php echo $company['House']; ?>';

                        postOfficeCode.type = 'int';
                        postOfficeCode.value = '<?php echo $company['Post_office_code']; ?>';
                        postOfficeCode.placeholder = '<?php echo $company['Post_office_code']; ?>';

                        <?php
                            $currentCityID = $company['City_id'];
                    }

                    $sql = "SELECT City_id, Name FROM Cities";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            $selected = ($row['City_id'] == $currentCityID) ? 'selected' : '';
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['City_id']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
                            option.selected = <?php echo $selected ? 'true' : 'false'; ?>;
                            citySelect.appendChild(option);
                            <?php
                        }
                    }
                }
            ?>

            var inputCityName = document.createElement('input');
            inputCityName.type = 'text';
            inputCityName.placeholder = 'Enter new city name';
            inputCityName.name = 'new_City_name';
            inputCityName.style.display = 'none';

            var inputPostCode = document.createElement('input');
            inputPostCode.type = 'int';
            inputPostCode.placeholder = 'Enter city post code';
            inputPostCode.name = 'new_Post_code';
            inputPostCode.style.display = 'none';

            var inputPhoneCode = document.createElement('input');
            inputPhoneCode.type = 'int';
            inputPhoneCode.placeholder = 'Enter city phone code';
            inputPhoneCode.name = 'new_Phone_code';
            inputPhoneCode.style.display = 'none';

            citySelect.onchange = function() {
                if (citySelect.value === 'new') {
                    inputCityName.style.display = 'block';
                    inputPostCode.style.display = 'block';
                    inputPhoneCode.style.display = 'block';
                } else {
                    inputCityName.style.display = 'none';
                    inputPostCode.style.display = 'none';
                    inputPhoneCode.style.display = 'none';
                }
            };

            var editButton = document.createElement('button');
            editButton.textContent = 'Save';
            editButton.onclick = function() {
                if (citySelect.value === 'new' && inputCityName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a new city name');
                    return false;
                } else if (citySelect.value === 'new' && inputCityName.value.length > 255) {
                    event.preventDefault();
                    alert('City name must be <= 255 symbols');
                    return false;
                } else if (citySelect.value === 'new' && inputPostCode.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a city post code');
                    return false;
                } else if (citySelect.value === 'new' && inputPhoneCode.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a city phone code');
                    return false;
                } else if (citySelect.value === 'new' && (isNaN(inputPostCode.value) || inputPostCode.value > 999 || inputPostCode.value < 100)) {
                    event.preventDefault();
                    alert('Post code must be a number between 100 and 999');
                    return false;
                } else if (citySelect.value === 'new' && (isNaN(inputPhoneCode.value) || inputPhoneCode.value > 999 || inputPhoneCode.value < 100)) {
                    event.preventDefault();
                    alert('Phone code must be a number between 100 and 999');
                    return false;
                } else if (house.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a house');
                    return false;
                } else if (shortName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a short name');
                    return false;
                } else if (fullName.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a full name');
                    return false;
                } else if (street.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a street');
                    return false;
                } else if (postOfficeCode.value.trim() === '') {
                    event.preventDefault();
                    alert('Please enter a post office code');
                    return false;
                } else if (isNaN(postOfficeCode.value) || postOfficeCode.value > 999 || postOfficeCode.value < 100) {
                    event.preventDefault();
                    alert('Post office code must be a number between 100 and 999');
                    return false;
                }

                var cityName = citySelect.value === 'new' ? inputCityName.value : '';
                var cityID = citySelect.value === 'new' ? -1 : citySelect.value;
                var postCode = citySelect.value === 'new' ? inputPostCode.value : -1;
                var phoneCode = citySelect.value === 'new' ? inputPhoneCode.value : -1;

                var form = document.createElement('form');
                form.method = 'post';
                form.action = 'edit_company.php';

                var companyIDInput = document.createElement('input');
                companyIDInput.type = 'hidden';
                companyIDInput.name = 'Company_id';
                companyIDInput.value = '<?php echo $companyID; ?>';

                var cityNameInput = document.createElement('input');
                cityNameInput.type = 'hidden';
                cityNameInput.name = 'City_name';
                cityNameInput.value = cityName;

                var cityIDInput = document.createElement('input');
                cityIDInput.type = 'hidden';
                cityIDInput.name = 'City_id';
                cityIDInput.value = cityID;

                var postCodeInput = document.createElement('input');
                postCodeInput.type = 'hidden';
                postCodeInput.name = 'Post_code';
                postCodeInput.value = postCode;
                    
                var phoneCodeInput = document.createElement('input');
                phoneCodeInput.type = 'hidden';
                phoneCodeInput.name = 'Phone_code';
                phoneCodeInput.value = phoneCode;

                var shortNameInput = document.createElement('input');
                shortNameInput.type = 'hidden';
                shortNameInput.name = 'Short_name';
                shortNameInput.value = shortName.value;

                var fullNameInput = document.createElement('input');
                fullNameInput.type = 'hidden';
                fullNameInput.name = 'Full_name';
                fullNameInput.value = fullName.value;

                var streetInput = document.createElement('input');
                streetInput.type = 'hidden';
                streetInput.name = 'Street';
                streetInput.value = street.value;

                var houseInput = document.createElement('input');
                houseInput.type = 'hidden';
                houseInput.name = 'House';
                houseInput.value = house.value;

                var postOfficeCodeInput = document.createElement('input');
                postOfficeCodeInput.type = 'hidden';
                postOfficeCodeInput.name = 'Post_office_code';
                postOfficeCodeInput.value = postOfficeCode.value;

                form.appendChild(companyIDInput);
                form.appendChild(cityNameInput);
                form.appendChild(cityIDInput);
                form.appendChild(postCodeInput);
                form.appendChild(phoneCodeInput);
                form.appendChild(shortNameInput);
                form.appendChild(fullNameInput);
                form.appendChild(streetInput);
                form.appendChild(houseInput);
                form.appendChild(postOfficeCodeInput);
                document.body.appendChild(form);
                form.submit();
            };

            container.appendChild(shortName);
            container.appendChild(fullName);
            container.appendChild(citySelect);
            container.appendChild(street);
            container.appendChild(house);
            container.appendChild(postOfficeCode);
            container.appendChild(inputCityName);
            container.appendChild(inputPostCode);
            container.appendChild(inputPhoneCode);
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
                $shortName = $_POST['Short_name'];
                $fullName = $_POST['Full_name'];
                $cityID = $_POST['City_id'];
                $cityName = $_POST['City_name'];
                $postCode = $_POST['Post_code'];
                $phoneCode = $_POST['Phone_code'];
                $street = $_POST['Street'];
                $house = $_POST['House'];
                $postOfficeCode = $_POST['Post_office_code'];

                $pdo->beginTransaction();

                try {
                    // add a city
                    if ($cityID == -1) {
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
                            $cityID = $stmt->fetch(PDO::FETCH_ASSOC)["City_id"];
                        } else {
                            $pdo->rollBack();
                            echo "ERROR: cannot add city";
                        }
                    }

                    // check last address_id
                    $sql = "SELECT Address_id FROM Companies WHERE Company_id = :companyID";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                    $stmt->execute();
                    if ($stmt->rowCount() > 0) {
                        $lastAddressID = $stmt->fetch(PDO::FETCH_ASSOC)["Address_id"];
                        $sql1 = "SELECT COUNT(*) as count FROM Companies WHERE Address_id = :addressID";
                        $stmt1 = $pdo->prepare($sql1);
                        $stmt1->bindParam(':addressID', $lastAddressID, PDO::PARAM_INT);
                        $stmt1->execute();
                        if ($stmt1->rowCount() > 0) {
                            $addressCount = $stmt1->fetch(PDO::FETCH_ASSOC)["count"];
                        }
                    }

                    // get address_id (get or create and get)
                    $sql = "SELECT Address_id FROM Addresses
                            WHERE Street = :street AND House = :house AND City_id = :cityID AND Post_office_code = :postOfficeCode";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':cityID', $cityID, PDO::PARAM_INT);
                    $stmt->bindParam(':street', $street, PDO::PARAM_STR);
                    $stmt->bindParam(':house', $house, PDO::PARAM_STR);
                    $stmt->bindParam(':postOfficeCode', $postOfficeCode, PDO::PARAM_INT);
                    $stmt->execute();
                    if ($stmt->rowCount() > 0) {
                        $addressID = $stmt->fetch(PDO::FETCH_ASSOC)["Address_id"];
                    } else {
                        $sql = "INSERT INTO Addresses (Street, House, City_id, Post_office_code) 
                                VALUES (:street, :house, :cityID, :postOfficeCode)";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':cityID', $cityID, PDO::PARAM_INT);
                        $stmt->bindParam(':street', $street, PDO::PARAM_STR);
                        $stmt->bindParam(':house', $house, PDO::PARAM_STR);
                        $stmt->bindParam(':postOfficeCode', $postOfficeCode, PDO::PARAM_INT);
                        $stmt->execute();

                        $sql = "SELECT Address_id FROM Addresses
                                WHERE Street = :street AND House = :house AND City_id = :cityID AND Post_office_code = :postOfficeCode";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':cityID', $cityID, PDO::PARAM_INT);
                        $stmt->bindParam(':street', $street, PDO::PARAM_STR);
                        $stmt->bindParam(':house', $house, PDO::PARAM_STR);
                        $stmt->bindParam(':postOfficeCode', $postOfficeCode, PDO::PARAM_INT);
                        $stmt->execute();
                        if ($stmt->rowCount() > 0) {
                            $addressID = $stmt->fetch(PDO::FETCH_ASSOC)["Address_id"];
                        }
                    }

                    $sql = "UPDATE Companies SET
                            Short_name = :shortName, 
                            Full_name = :fullName, 
                            Address_id = :addressID
                            WHERE Company_ID = :companyID";
                    $stmt = $pdo->prepare($sql);
                    $stmt->bindParam(':shortName', $shortName, PDO::PARAM_STR);
                    $stmt->bindParam(':fullName', $fullName, PDO::PARAM_STR);
                    $stmt->bindParam(':addressID', $addressID, PDO::PARAM_INT);
                    $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                    $stmt->execute();

                    // delete last address
                    if ($addressCount == 1 && $lastAddressID != $addressID) {
                        $sql = "DELETE FROM Addresses WHERE (Address_id = :addressID)";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':addressID', $lastAddressID, PDO::PARAM_INT);
                        $stmt->execute();
                    }

                    $pdo->commit();
                    echo "The company is successfully updated";
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
        } else {
            echo "EDIT FORM<br>";
            echo "<div id='editform-container'></div><br><br>";
            ?>
            <script>
                editCompanyInput();
            </script>
            <?php
                echo "<form id='redirectForm' action='../company.php' method='get'>
                            <input type='hidden' name='Company_id' value='" . $companyID . "'>
                            <button type='submit'>Return</button>
                        </form>";
        }
    ?>
</body>
</html>