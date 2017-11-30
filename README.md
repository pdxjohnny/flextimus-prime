# Flextimus Prime

Posture monitor

## Getting Started

Make sure you have git configured.

```console
git config --global user.name 'Frist Last'
git config --global user.email 'first.last@example.com'
```

Install the git commit hooks. This will ensure that you can only commit if the
source compiles successfully.

> **WARNING** This adds `set auto-load safe-path /` to your `~/.gdbinit` which
> is not safe because when you start gdb it will now execute any commands in the
> `.gdbinit` file in the current directory.

```console
./scripts/install.sh
```

Find out what you should work on

```console
git todo
```

> This will be run every time you pull so that see todos added by others.

Create a new branch for your work

```console
git checkout -b describe_my_intended_changes
```

Now connect the board, and start `st-util`. From the the top level of this repo
run `arm-eabi-none-gdb` or `make gdb`.

Start hacking!

### Dependencies

1. [mbed](https://os.mbed.com/docs/v5.6/tools/setup.html)

## Contributing

See [CONTRIBUTING.md](https://github.com/pdxjohnny/flextimus-prime/blob/master/CONTRIBUTING.md)

## Team

- Max (lead)
- Julian
- John
- fouric

> Fall 2017
> Practicum Project

## Description

Your team will be responsible for designing, constructing, and demonstrating a small project as part of the course. The project is intended to both give you some necessary implementation skills and experience as well as serve as a model for your capstone experience. In fact, the capstone evaluation forms and worksheets will be used in evaluating and grading your practicum project.

You will get experience with PCB schematic and layout tools, project documentation and management tools, revision control systems, PCB assembly, and (depending upon your project) microcontroller IDEs.

Equally important, you will create project requirements, test plans, schedules, and documentation as you would for your capstone project (or a project in industry). Requirements Your project can do almost anything but must satisfy the following course requirements:

- Have one or more inputs or sensors
  - e.g. photodiodes, switches, accelerometers, pressure sensor, network data (wired or wireless), etc.

- Have one or more outputs or transducers
  - e.g. LEDs, LCDs, motors, networking (wired or wireless).

- Have one or more processing modules which control some of the outputs based on the inputs
  - e.g. analog filters, microcontrollers, FPGAs, etc.

- Use a two layer PCB that is between 1 and 16 in 2, with no side of the board being less than 1 inch or more than 12 inches.

- Use components that can be hand soldered or easily soldered in a crude reflow oven (no BGAs, uBGAs, LGAs, or components smaller than 0603 packages allowed)

## Grading Criteria

Your project will be evaluated using the capstone evaluation forms found on the capstone web site.

Portions of your practicum are evaluated separately throughout the course as part of the homework assignments.

Other aspects are evaluated as part of your project demonstration (final project presentation, design documentation, communication (e.g. weekly progress reports)). Originality Ideally, you should choose an original project idea or a new implementation of an existing idea. To receive an A on the practicum project you must not use existing work (unless specifically approved in advance by the instructor in writing).

If you choose to implement an existing design or incorporate existing work (including but not limited to schematics, layout, or code), your maximum achievable grade is a B.

However, you must disclose both what you've borrowed and the source.

Use of existing work that is not disclosed will result in a practicum grade of F. Recommendation It's far more important that your project be complete and functioning than complex. It's essential that you complete all aspects of the project from idea generation through requirements definition, design description/modeling, layout, construction, test, debug, and documentation.

It's important that you demonstrate a working project. A simple project designed and implemented completely by the team using the disciplined design process taught in class and demonstrably working will beat out an overly ambitious project that's incomplete, non-functioning, or haphazardly implemented.

Consider this in choosing a project. That's not to suggest you shouldn't be imaginative or ambitious. Everything else being equal, a creative points.
