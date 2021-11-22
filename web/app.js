var express = require('express');
var app = express();
const port = 80;

app.use(express.static('public'));
app.get('/', (req,res) =>{
    var options = {
        root : path.join(__dirname,'public'),
    };

    res.sendFile('index.html',options);
});

app.listen(port);
