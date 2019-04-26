function db_get_student_info() {
    jQuery.get({
        url: 'http://localhost:80/tables/StudentInfo/'
    }, function (data) {
        let dataParse = JSON.parse(data);
        if ($('#Table_Students').length) {
            $('#Table_Students').remove();
        }
        $('.Tables').append(
            `<table id="Table_Students">
                <tr>
                <th>Index</th>
                <th>First name</th>
                <th>Last name</th>
                </tr>
            </table>`
        );
        dataParse['elements'].forEach(function (elem) {
            $('#Table_Students').append(
                '<tr>' +
                '<td>' + elem['SID'] + '</td>' +
                '<td>' + elem['fName'] + '</td>' +
                '<td>' + elem['lName'] + '</td>' +
                '</tr>'
            );
        })
    });
}

function db_get_presence() {
    jQuery.get({
        url: 'http://localhost:80/tables/'
    }, function (data) {
        let dataParse = JSON.parse(data);
        if ($('#Table_Presence').length) {
            $('#Table_Presence').remove();
        }
        $('.Tables').append(
            `<table id="Table_Presence">
                <tr>
                <th>Index</th>
                <th>Date</th>
                <th>Course Name</th>
                <th>Room</th>
                </tr>
            </table>`
        );
        dataParse['elements'].forEach(function (elem) {
            $('#Table_Presence').append(
                '<tr>' +
                '<td>' + elem['SID'] + '</td>' +
                '<td>' + elem['Date'] + '</td>' +
                '<td>' + elem['sName'] + '</td>' +
                '<td>' + elem['Room'] + '</td>' +
                '</tr>'
            );
        })
    });
}

function db_index_exists(index) {
    jQuery.get({
        url: 'http://localhost:80/tables/StudentInfo/' + index.toString()
    }, function (data, textStatus, jqXHR) {
        return jqXHR.status !== 200;
    });
}

function db_add_presence(index, CID, room) {
    if (!isNaN(index) && index.toString().indexOf('.') === -1) {
        let jsonData = '{"SID":' + index + ',"CID":' + CID.toString() + ',"Room":"' + room + '"}';
        jQuery.post({
            url: 'http://localhost:80/tables/Presence/',
            dataType: "json",
            contentType: "application/json",
            data: jsonData
        }, function (data, textStatus, jqXHR) {
            return jqXHR.status === 200;
        });
    }
}

function db_add_row(index, fName, lName) {
    if (!isNaN(index) && index.toString().indexOf('.') === -1) {
        if (fName.length !== 0 && lName.length !== 0) {
            let jsonData = '{"SID":' + index + ',"fName":"' + fName + '","lName":"' + lName + '"}';
            jQuery.post({
                url: 'http://localhost:80/tables/StudentInfo/',
                dataType: "json",
                contentType: "application/json",
                data: jsonData
            }, function (data, textStatus, jqXHR) {
                db_get_student_info();
                return jqXHR.status === 200;
            });
        }
    }
}

function db_delete_row(index) {
    $.ajax({
        url: 'http://localhost:80/tables/StudentInfo/' + index.toString(),
        type: 'DELETE'
    });
}

function popups_fadeout() {
    let popups = $('.Popups_Container');
    let content = $('.Content_Container');
    popups.stop();
    content.stop();
    content.fadeTo(1000, 1);
    popups.fadeOut(500);
}

function popups_fadein(title, window_function) {
    let popups = $('.Popups_Container');
    let content = $('.Content_Container');
    window_function(title);
    content.stop();
    popups.stop();
    content.fadeTo(1000, 0.5);
    popups.fadeIn(500);
}

function popups_inputs_add(title) {
    let inputs = $('.Inputs');
    inputs.empty();
    inputs.append('<p id="Inputs_Title">' + title + '</p>');
    inputs.append('<label for="Input_SID"></label><input type="number" name="SID" placeholder="Index" id="Input_SID">');
    inputs.append('<label for="Input_fName"></label><input type="text" name="fName" placeholder="First name" id="Input_fName">');
    inputs.append('<label for="Input_lName"></label><input type="text" name="lName" placeholder="Last name" id="Input_lName">');
    inputs.css('height', '140px');
    inputs.css('width', '200px');
    $('#Cancel').css('left', '96px');
}

function popups_inputs_delete(title) {
    let inputs = $('.Inputs');
    inputs.empty();
    inputs.append('<p id="Inputs_Title">' + title + '</p>');
    inputs.append('<label for="Input_SID"></label><input type="number" name="SID" placeholder="Index" id="Input_SID">');
    inputs.css('height', '80px');
    inputs.css('width', '250px');
    $('#Cancel').css('left', '121px');
}