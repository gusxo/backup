const express = require('express');
const app = express();
const port_http = 80;
const port = 8080;
const http = require('http');
const https = require('https');
const path = require('path');
const fs = require('fs');
const readline = require('readline')

const option = {
    key: fs.readFileSync('/home/gusxo/Projects/linuxprograming/key.pem'),
    cert: fs.readFileSync('/home/gusxo/Projects/linuxprograming/cert.pem')
};

//pw file read
const userInfo = {};
const readpw = async ()=>{
    const pwfile = fs.createReadStream(`${__dirname}/pw`);
    const rl = readline.createInterface({
        input: pwfile,
        crlfDelay: Infinity
    });
    for await (const line of rl){
        const blank = line.indexOf(' ');
        const id = line.substr(0, blank);
        const pw = line.substr(blank+1);
        userInfo[id] = pw;
        console.log(`read id-password pair : { ${id} / ${userInfo[id]} }`);
    }
}
readpw();

http.createServer(app).listen(port_http, ()=>{
    console.log(`Http server listing on port ${port}`);
});
https.createServer(option, app).listen(port, ()=>{
    console.log(`Https server listing on port ${port}`);
});

app.use(express.json());
app.get('/', (req,res)=>{
    res.status(200).sendFile(`${__dirname}/index.html`);
});

app.post('/submit', (req,res)=>{
    console.log(`${req.body.id} / ${req.body.pw} / ${req.body.value}`);
    const id = req.body.id;
    const pw = req.body.pw;
    const val = req.body.value;
    if(userInfo[id]){
        if(userInfo[id] == pw){
            if(isNaN(val)){
                res.send({msg:"value is NaN"});
                console.log("post result : val error");
                return;
            }
            res.send({msg: val*val});
            console.log("post result : success");
            return;
        }
    }
    res.send({msg:"auth failed"});
    console.log("post result : auth failed");
});