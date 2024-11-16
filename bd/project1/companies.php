<!DOCTYPE html>
<head>
    <meta charset='utf-8'>
</head>
<title>Companies</title>
<!-- список компаний, их названия и количество специализаций + кнопка удаления для каждого + все ссылками на личные страницы
надо сортировку по названиям кратким, количеству специализаций
и фильтрацию по части названия
кнопка добавления фирмы -->
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

            var inShortName = document.createElement('input');
            inShortName.placeholder = 'In short name';

            var setButton = document.createElement('button');
            setButton.textContent = 'Set';
            setButton.onclick = function() {
                var form = document.createElement('form');
                form.method = 'get';
                form.action = 'companies.php';

                var sortInput = document.createElement('input');
                sortInput.type = 'hidden';
                sortInput.name = 'Sort_by';
                sortInput.value = sortSelect.value;

                var inShortNameInput = document.createElement('input');
                inShortNameInput.type = 'hidden';
                inShortNameInput.name = 'inShortName';
                inShortNameInput.value = inShortName.value;

                form.appendChild(sortInput);
                form.appendChild(inShortNameInput);
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
            container.appendChild(inShortName);
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

                    $companyID = $_POST['Company_id'];

                    $pdo->beginTransaction();

                    try {
                        $sql = "SELECT Count(*) as count FROM People WHERE Company_id = :companyID";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                        $stmt->execute();

                        $count = 0;
                        if ($stmt->rowCount() > 0) {
                            $count = $stmt->fetch(PDO::FETCH_ASSOC)["count"];
                        }
                        if ($count != 0) {
                            $pdo->commit();
                            echo "The company cannot be deleted, because some people work in this company";
                        } else {
                            // if a specialization does not belong to any company or person delete it (get this specializations)
                            $sql1 = "SELECT Sp_id FROM Specializations_of_companies WHERE Company_id = :companyID";
                            $stmt1 = $pdo->prepare($sql1);
                            $stmt1->bindParam(':companyID', $companyID, PDO::PARAM_INT);
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

                            //delete a company from specializations_of_companies
                            $sql = "DELETE FROM Specializations_of_companies WHERE Company_id = :companyID";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                            $stmt->execute();

                            //delete an address
                            $sql = "SELECT Address_id FROM Companies WHERE Company_id = :companyID";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                            $stmt->execute();
                            if ($stmt->rowCount() > 0) {
                                $addressID = $stmt->fetch(PDO::FETCH_ASSOC)["Address_id"];
                                $sql1 = "SELECT COUNT(*) as count FROM Companies WHERE Address_id = :addressID";
                                $stmt1 = $pdo->prepare($sql1);
                                $stmt1->bindParam(':addressID', $addressID, PDO::PARAM_INT);
                                $stmt1->execute();
                                if ($stmt1->rowCount() > 0) {
                                    $addressCount = $stmt1->fetch(PDO::FETCH_ASSOC)["count"] == 1;
                                }
                            }

                            //delete a company from companies
                            $sql = "DELETE FROM Companies WHERE Company_id = :companyID";
                            $stmt = $pdo->prepare($sql);
                            $stmt->bindParam(':companyID', $companyID, PDO::PARAM_INT);
                            $stmt->execute();

                            //delete an address
                            if ($addressCount == 1) {
                                $sql = "DELETE FROM Addresses WHERE (Address_id = :addressID)";
                                $stmt = $pdo->prepare($sql);
                                $stmt->bindParam(':addressID', $addressID, PDO::PARAM_INT);
                                $stmt->execute();
                            }

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
                            echo "The company is successfully deleted";
                        }
                    } catch (PDOException $e) {
                        $pdo->rollBack();
                        echo "ERROR: " . $e->getMessage();
                    }

                    echo "<form id='redirectForm' action='../companies.php' method='get'>
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
    
                        $sql = "INSERT INTO Companies (Short_name, Full_name, Address_id) VALUES
                                (:shortName, :fullName, :addressID)";
                        $stmt = $pdo->prepare($sql);
                        $stmt->bindParam(':shortName', $shortName, PDO::PARAM_STR);
                        $stmt->bindParam(':fullName', $fullName, PDO::PARAM_STR);
                        $stmt->bindParam(':addressID', $addressID, PDO::PARAM_INT);
                        $stmt->execute();
    
                        $pdo->commit();
                        echo "The company is successfully added";
                    }
                    catch (PDOException $e) {
                        $pdo->rollBack();
                        echo "ERROR: " . $e->getMessage();
                    }
                    echo "<form id='redirectForm' action='../companies.php' method='get'>
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
                        $orderBy = 'ORDER BY Short_name ASC';
                        break;
                    case 'Sp_count':
                        $orderBy = 'ORDER BY count DESC';
                        break;
                    default:
                        $orderBy = '';
                }

                
                $shortNameFilter = isset($_GET['inShortName']) ? $_GET['inShortName'] : '';

                $whereClause = [];
                if (!empty($shortNameFilter)) {
                    $whereClause[] = "Short_name LIKE '%$shortNameFilter%'";
                }

                $whereClause = !empty($whereClause) ? 'WHERE ' . implode(' AND ', $whereClause) : '';

                $sql = "WITH sp_count AS (
                            SELECT Company_id, Count(*) as count
                            FROM Specializations_of_companies
                            GROUP BY Company_id
                        )
                        SELECT c.Company_id, Short_name, Full_name, count
                        FROM Companies c
                        LEFT JOIN sp_count sp ON c.Company_id = sp.Company_id
                        $whereClause
                        $orderBy";
                $stmt = $pdo->prepare($sql);
                $stmt->execute();
                
                if ($stmt->rowCount() > 0) {
                    echo "<p>Companies (and number of their specialties):</p>";
                    echo '<ol>';
                    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                        $count = $row['count'];
                        if ($count == NULL) {
                            $count = 0;
                        }
                        echo "<li>
                                <a href= \"company.php?Company_id=  ${row['Company_id']} \"> ${row['Short_name']}</a>
                                "  . str_repeat('&nbsp;', 5) . "${row['Full_name']}
                                "  . str_repeat('&nbsp;', 5) . "$count
                                <form action='companies.php' method='post' style='display:inline;'>
                                        <input type='hidden' name='Method' value='delete'>
                                        <input type='hidden' name='Company_id' value='${row['Company_id']}'>
                                        <button type='submit'>Delete</button>
                                </form>
                            </li>";
                    }
                    echo '</ol>';
                } else {
                    echo "table is empty";
                }
            };

            echo "<div id='new-company-container'>
                    <button id='add-company-btn' onclick='addCompanyInput()'>Add Company</button>
                    <div id='company-input-container'></div>
                </div><br><br>";
        }
    ?><br>
    <script>
        function addCompanyInput() {
            var container = document.getElementById('company-input-container');
                
            var shortName = document.createElement('input');
            shortName.placeholder = 'Enter short name';
            shortName.required = true;
            var fullName = document.createElement('input');
            fullName.placeholder = 'Enter full name';
            fullName.required = true;
            var street = document.createElement('input');
            street.placeholder = 'Enter street';
            street.required = true;
            var house = document.createElement('input');
            house.placeholder = 'Enter house';
            house.required = true;
            var postOfficeCode = document.createElement('input');
            postOfficeCode.placeholder = 'Enter post office code';
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

                    $sql = "SELECT City_id, Name FROM Cities";
                    $stmt = $pdo->prepare($sql);
                    $stmt->execute();
                        
                    if ($stmt->rowCount() > 0) {
                        while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
                            ?>
                            var option = document.createElement('option');
                            option.value = '<?php echo $row['City_id']; ?>';
                            option.textContent = '<?php echo $row['Name']; ?>';
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
            inputCityName.style.display = 'block';

            var inputPostCode = document.createElement('input');
            inputPostCode.type = 'int';
            inputPostCode.placeholder = 'Enter city post code';
            inputPostCode.name = 'new_Post_code';
            inputPostCode.style.display = 'block';

            var inputPhoneCode = document.createElement('input');
            inputPhoneCode.type = 'int';
            inputPhoneCode.placeholder = 'Enter city phone code';
            inputPhoneCode.name = 'new_Phone_code';
            inputPhoneCode.style.display = 'block';

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

            var addButton = document.createElement('button');
            addButton.textContent = 'Save';
            addButton.onclick = function() {
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
                form.action = 'companies.php';

                var methodInput = document.createElement('input');
                methodInput.type = 'hidden';
                methodInput.name = 'Method';
                methodInput.value = 'add';

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

                form.appendChild(methodInput);
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
            container.appendChild(addButton);

            document.getElementById('add-company-btn').style.display = 'none';
        }
    </script>
</body>
</html>