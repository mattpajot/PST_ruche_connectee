///
/// SERVEUR NodeJS POUR FAIRE FONCTIONNER LE SITE, AFIN DE FAIRE DES REQUÊTES SIMPLEMENT
///
/// PST RUCHE CONNECTEE - 2020/2021 3A
///

//Tous les includes nécessaires pour le fonctionnement du serveur NodeJS

var express = require('express');
var http = require('http');
var serve_static = require("serve-static");
var url = require('url');
var querystring = require('querystring');
var fs = require('fs');
var app = express();
//Voici les informations de notre API
// l'id :
var id = "2E6C54";
// notre login :
var username = "60a57bfe25643237d0aca872";
// notre mot de passe
var password = "4c1dd72a27b191fe5102cdf06701753d";


app.use(serve_static(__dirname+"/public"));

var serveur = http.Server(app);
serveur.listen(8080, function(){});

const request = require("request");

// Maintenant, petite différence, nous voulons récupérer les informations de l'API sigfox
// Ainsi, on utilise la méthode get pour les recevoir
// et nous entrons l'url de ce que nous voulons avoir
// ici, 2E6C54 est l'id de notre produit et 'messages' permet de récupérer tous les messages que nous voulons

var getAPIData = {
    method: "GET",
    url: "https://api.sigfox.com/v2/devices/2E6C54/messages",
    auth: {
        "user": username,
        "pass": password
    }
}

var database;

// Et c'est ici que nous récupérons les informations de l'API sous forme de database en format JSON, que nous savons donc bien utiliser :

request(getAPIData, function (error, response, body) {
    if (!error && response.statusCode == 200) {
        database = JSON.parse(body);
    }
})

// Première fonction get pour recevoir tous les messages

app.get('/messages', function (req, res){

    res.send(database.data);
});

// Seconde pour recevoir les messages d'un device en particulier (:device prendra la valeur d'un id)

app.get('/messages/:device', function(req, res){

	let id_device = req.params.device;
	let obj = []
	console.log(id_device);

	for(let i = 0; i < database.data.length; i++)
	{
		if(database.data[i].device.id == id_device)
		{
			obj.push(database.data[i]);
		}
	}
	res.send(obj);
});
