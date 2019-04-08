function db_get_content() {
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
                <th>Firstname</th>
                <th>Lastname</th>
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
                db_get_content();
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
