// server.js

// BASE SETUP
// =============================================================================

// call the packages we need
var express    = require('express');        // call express
var app        = express();                 // define our app using express
var bodyParser = require('body-parser');
var shell      = require('node-cmd');
var tcp        = require('tcp-proxy');

var path = __dirname
var bulbOn = true;

var server = tcp.createServer({
  target: {
    host: '127.0.0.1',
    port: 9090
  }
});

server.listen(9091);

// configure app to use bodyParser()
// this will let us get the data from a POST
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

var port = process.env.PORT || 9090;        // set our port

// ROUTES FOR OUR API
// =============================================================================
var router = express.Router();              // get an instance of the express Router
var count = 0;

router.use(function(req, res, next) {
    console.log('Something is happening.');
    next();
}, bodyParser.json());

router.get('/', function(req, res, err) {
	res.sendFile(path + "/index.html");
});

router.get('/styles.css', function(req, res, err) {
	res.sendFile(path + "/styles.css");
});

router.get('/spam', function(req, res, err) {
	res.sendFile(path + "/spam.html");
});

router.get('/images/:image', function(req, res, err) {
	res.sendFile(path + "/images/" + req.params.image);
});

var f='data.csv',
    fs=require('fs');

// fs.writeFile(f,'Some text to write.',function(err){
//   if(err)
//     console.error(err);
//   console.log('Written!');
// });

// +new Date

var presentCost = 0;

// var lastLine = require('last-line');

// var fs = require('fs'); // file system module
/*
fs.readFile(f, 'utf-8', function(err, data) {
    if (err) throw err;

    var lines = data.trim().split('\n');
    var lastLine = lines.slice(-1)[0];

    var fields = lastLine.split(',');
    var audioFile = fields.slice(-1)[0].replace('file:\\\\', '');

    presentCost = parseInt(audioFile)
    console.log(presentCost)
});*/

router.get('/turnOff', function(req, res, next) {
	bulbOn = !bulbOn;
	console.log('bulb toggle');
});
var sliderValue = 15;
router.get('/slider/:value',function(req, res, next){
	console.log('Slider value = ' + req.params.value);
	sliderValue = req.params.value;
	res.sendStatus(200);
})

var resstring;
router.post('/api', function(req, res, next) {
	// var received = req.body.Value;
	// console.log('Recieved data' + req.body.current);
	console.log('esphere!');
	if(bulbOn){
		resstring = sliderValue + " 1";
		res.json(resstring);
	} else {
		resstring = sliderValue + " 0";
		res.json(resstring);
	}

});

// more routes for our API will happen here

// REGISTER OUR ROUTES -------------------------------
// all of our routes will be prefixed with /api
app.use('/', router);

// START THE SERVER
// =============================================================================
app.listen(port);
console.log('Magic happens on port ' + port);
