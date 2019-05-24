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

function db_add_row() {
    let index = parseInt($('#Input_SID').val());
    let fName = $('#Input_fName').val();
    let lName = $('#Input_lName').val();

    if (!isNaN(index) && index.toString().indexOf('.') === -1) {
        if (fName.length !== 0 && lName.length !== 0) {
            let jsonData = '{"SID":' + index + ',"fName":"' + fName + '","lName":"' + lName + '"}';
            jQuery.post({
                url: 'http://localhost:80/tables/StudentInfo/',
                dataType: "json",
                contentType: "application/json",
                data: jsonData
            }).fail(function (response) {
                if (response['status'] === 201) {
                    db_get_student_info();
                    alert('Student added successfully.');
                } else if (response['status'] === 409) {
                    alert('Student with given id already exists.');
                }
            });
        }
    }
}

function db_delete_row() {
    let index = parseInt($('#Input_SID').val());

    $.ajax({
        url: 'http://localhost:80/tables/StudentInfo/?sid=' + index.toString(),
        type: 'DELETE',
        success: function () {
            db_get_student_info();
            alert('Student deleted successfully.');
        },
        error: function () {
            alert('Delete failed.');
        }
    });
}

function popup_fadeout(finish) {
    let popups = $('.Popups_Container');
    let content = $('.Content_Container');
    popups.stop();
    content.stop();
    content.fadeTo(1000, 1);
    popups.fadeOut(500, finish);
}

function popup_fadein(title, window_function) {
    let popups = $('.Popups_Container');
    let content = $('.Content_Container');
    window_function(title);
    content.stop();
    popups.stop();
    content.fadeTo(1000, 0.5);
    popups.fadeIn(500);
}


function add_button_listeners(click_func) {
    let button = $('#But_Execute');
    button.on("mouseenter", function () {
        $(this).css({'backgroundColor': "lightsteelblue"});
    });
    button.on("mouseleave", function () {
        $(this).css({'backgroundColor': "white"});
    });
    button.on("click", function () {
        click_func();
    });
}

function popup_inputs_add(title) {
    let popups = $('.Popups_Container');
    let inputs = $('.Inputs');
    inputs.empty();
    inputs.append('<p id="Inputs_Title">' + title + '</p>');
    inputs.append('<input type="number" name="SID" placeholder="Index" id="Input_SID">');
    inputs.append('<input type="text" name="fName" placeholder="First name" id="Input_fName">');
    inputs.append('<input type="text" name="lName" placeholder="Last name" id="Input_lName">');
    inputs.append('<button id="But_Execute">Execute</button>');
    inputs.css('height', '160px');
    inputs.css('width', '200px');
    popups.css('height', '170px');
    popups.css('width', '210px');
    $('#Cancel').css('left', '185px');

    add_button_listeners(db_add_row);
}

function popup_inputs_delete(title) {
    let popups = $('.Popups_Container');
    let inputs = $('.Inputs');
    inputs.empty();
    inputs.append('<p id="Inputs_Title">' + title + '</p>');
    inputs.append('<label for="Input_SID"></label><input type="number" name="SID" placeholder="Index" id="Input_SID">');
    inputs.append('<button id="But_Execute">Execute</button>');
    inputs.css('height', '100px');
    inputs.css('width', '250px');
    popups.css('height', '110px');
    popups.css('width', '260px');
    $('#Cancel').css('left', '235px');

    add_button_listeners(db_delete_row);
}

function popup_inputs_add_presence(title) {
    let popups = $('.Popups_Container');
    let inputs = $('.Inputs');
    inputs.empty();
    inputs.append('<p id="Inputs_Title">' + title + '</p>');
    inputs.append('<label for="Input_SID"></label><input type="number" name="SID" placeholder="Index" id="Input_SID">');
    inputs.append('<label for="Input_CID"></label><input type="number" name="CID" placeholder="Course ID" id="Input_CID">');
    inputs.append('<input type="text" name="Room" placeholder="Room" id="Input_Room">');
    inputs.append('<button id="But_Execute">Execute</button>');
    inputs.css('height', '160px');
    inputs.css('width', '250px');
    popups.css('height', '170px');
    popups.css('width', '260px');
    $('#Cancel').css('left', '235px');
    $("#Input_CID").css('top','70px');
    $("#Input_Room").css('top','100px');

    add_button_listeners(db_delete_row);
}
