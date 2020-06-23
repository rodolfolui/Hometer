# Hometer

[![License](https://img.shields.io/badge/License-Apache2-blue.svg)](https://www.apache.org/licenses/LICENSE-2.0) [![Slack](https://img.shields.io/badge/Join-Slack-blue)](https://callforcode.org/slack) [![Website](https://img.shields.io/badge/View-Website-blue)](https://code-and-response.github.io/Project-Sample/)

Measurement of carbon footprint in homes and calculating in a reliable way in a distributed blockchain.  The variables to measure includes but not limited to energy, water consumption, CO/CO2.

*Read this in other languages: [English](README.md), [Español](README.es_co.md).*

## Contents

1. [Short description](#short-description)
1. [Demo video](#demo-video)
1. [The architecture](#the-architecture)
1. [Long description](#long-description)
1. [Project roadmap](#project-roadmap)
1. [Getting started](#getting-started)
1. [Running the tests](#running-the-tests)
1. [Live demo](#live-demo)
1. [Built with](#built-with)
1. [Contributing](#contributing)
1. [Versioning](#versioning)
1. [Authors](#authors)
1. [License](#license)

## Short description

### What's the problem?

The calculation of carbon footprint is not the same for all the locations and human activities, it depends on how the energy or water is produced (is different for example for hydraulic than gas generators), and could be subject to human intervention.

We considered the measurement should be obtained by home automation and the carbon footprint must be calculated taking into account the aspects related with the conditions used to do this calculation.  All the aspects considered need to be validated by the participants and registered in a blockchain in order to have a realistic and reliable carbon 

### How can technology help?

The IoT devices to be used at home need to provide some aditional value than being only sensors, and be able to read and send the data in an automated way.

### The idea

The IoT devices used at home or work need to be non intrusive and cost efficient, the data collected by the devices need to be integrated with calculations depending on the location of the sensor and all the calculation need to count for the home of office account owner; so we have several actors updating the ledger for carbon footprint.  The hometer network provides all of this capabilities.

## Demo video

[![Watch the video](https://github.com/rodolfolui/Hometer/blob/master/docs/img/TeamHometer.png)](https://youtu.be/vOgCOoy_Bx0)

## The architecture

![Video transcription/translation app](https://github.com/rodolfolui/Hometer/blob/master/docs/img/IoT-Blockchain.png)

1. The device collects consumption info and reports to the IoT network
2. The user can connect to the device by WiFi to get information about configuration and mettering on real time
3. The NodeRed App collects multiple device information and correlates the location of the device to determine the calculation for carbon footprint
4. The user connect to NodeRed dashboard to view historical data consumption and hours of consumption, useful in determining the time of the day and the consumptions in the day.
5. The calculated consumption is sent to hyperledger to create a carbon footprint record for person ledger
6. Peer uses an “endorsement” service to simulate the proposed transaction against the relevant smart contracts. This service confirms that the transaction is possible, given the current state of the ledger. Examples of invalid proposals might be creating an asset that already exists, querying the state of an asset that does not exist, etc.

## Long description

[More detail is available here](DESCRIPTION.md)

## Project roadmap

![Roadmap](https://github.com/rodolfolui/Hometer/blob/master/docs/img/Roadmap.png)

## Getting started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

```bash
dnf install wget
wget http://www.example.com/install.sh
bash install.sh
```

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be, for example

```bash
export TOKEN="fffd0923aa667c617a62f5A_fake_token754a2ad06cc9903543f1e85"
export EMAIL="jane@example.com"
dnf install npm
node samplefile.js
Server running at http://127.0.0.1:3000/
```

And repeat

```bash
curl localhost:3000
Thanks for looking at Code-and-Response!
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why, if you were using something like `mocha` for instance

```bash
npm install mocha --save-dev
vi test/test.js
./node_modules/mocha/bin/mocha
```

### And coding style tests

Explain what these tests test and why, if you chose `eslint` for example

```bash
npm install eslint --save-dev
npx eslint --init
npx eslint sample-file.js
```

## Live demo

You can find a running system to test at [colnodered.mybluemix.net/](http://colnodered.mybluemix.net/) with user and password admin 

## Built with

* [IBM Cloudant](https://cloud.ibm.com/catalog?search=cloudant#search_results) - The NoSQL database used
* [NodeRed running on IBM CloudFoundry](https://cloud.ibm.com/catalog?search=%22node-red%20app%22#search_results) - The compute platform for handing cloud app logic
* [IBM Internet of Things Platform](https://cloud.ibm.com/catalog?search=internet%20of%20things%20platform#search_results) - The IBM Cloud service to manage the devices network
* [Node-RED Blockchain](https://github.com/johnwalicki/Node-RED-Storm-BlockChain-Demo) - Base code to build Blockchain App and guide to use the APIs

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [GitHub](http://github.com/) for versioning. For the versions available, see the [tags on this repository](https://github.com/rodolfolui/Hometer/tags).

## Authors

* **Alba Albarracin** 
* **Daniel Lema** 
* **Ivan Vasquez** 
* **Rodolfo Vasquez** 


See also the list of [contributors](https://github.com/rodolfolui/Hometer/graphs/contributors) who participated in this project.

## License

This project is licensed under the Apache 2 License - see the [LICENSE](LICENSE) file for details
