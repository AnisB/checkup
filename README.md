# checkup
A tool to display useful information on a raspberry pi (Traffic, Train, Weather, etc.)

# Instructions

Set up your personal info in the data/config.json file (tokens, locations).

To compile on windows run in the root folder

On Windows:

```bash
ruby make.rb --compiler vc15
```
or
```bash
ruby make.rb --compiler vc14
```


On Linux:

```bash
ruby make.rb --compiler makefile --platform linux
```
or
```bash
ruby make.rb -c makefile -p linux
```
