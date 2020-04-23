# Documentation Ideas

This section collects ideas to improve the coreboot documentation and
should serve as a pool of ideas for people who want to improve the current
documentation status of coreboot.

The main purpose of this document is to gather documentation ideas for technical
writers of the seasons of docs. Nevertheless anyone who wants to help improving
the current documentation situation can take one of the projects.

Each entry should outline what would be done, the benefit it brings
to the project, the pre-requisites, both in knowledge and parts. They
should also list people interested in supporting people who want to work
on them.

## Restructure Existing Documentation

The goal is to improve the user experience and structure the documentation more
logically. The current situation makes it very hard for beginners, but also for
experienced developers to find anything in the coreboot documentation.

One possible approach to restructure the documentation is to split it up such
that we divide the group of users into:

* (End-)users
Most probably users which _just_ want to use coreboot as fast as possible. This
section should include guidelines on how to build coreboot, how to flash coreboot
and also which hardware is currently supported.

* Developers
This section should more focus on the developer side-of-view. This section would
include how to get started developing coreboot, explaining the basic concepts of
coreboot and also give guideance on how to proceed after the first steps.

* Knowledge area
This section is very tighlight coupled to the developer section and might be merged
into it. The _Knowledge area_ can give a technical deep dive on various drivers,
technologies, etc.

* Community area
This section gives some room for the community: Youtube channels, conferences,
meetups, forums, chat, etc.

A [first approach](https://review.coreboot.org/c/coreboot/+/40327) has already been made here and might be a basis for the work.
Most of the documentation is already there, but scattered around the documentation
folder.

### Requirements
* Understanding on how a different groups of users might use the documentation area
* Basic understanding of how coreboot works (Can be worked out _on-the-fly_)

### Mentors
* christian.walter@9elements.com
* TBD

## Update Howto/Guides

An important part to involve new people in the project, either as developer or
as enduser, are guides and how-to's. There are already some guides which need
to be updated to work, and could also be extended to multiple platforms, like
Fedora or Arch-Linux. Also guidance for setting up coreboot with a Windows
environment would be helpful.

In addition, the vboot guidance needs an update/extensions, that the security
features within coreboot can be used by non-technical people.

For developers, how to debug coreboot and various debugging techniques need
documentation.

### Requirements
* Knowledge of virtual machines, how to install different OSs and set up the
  toolchain on different operating systems
* Knowledge of debugging tools like gdb

### Mentors
* christian.walter@9elements.com
* TBD

## How to Support a New Board

coreboot benefits from running on as many platforms as possible. Therefore we
want to encourage new developers on porting existing hardware to coreboot.
Guidance for those new developers need to be made such that they are able to
take the first steps supporting new mainboards, when the SoC support already
exists. There should be a 'how-to' guide for this. Also what are common problems
and how to solve those.

### Requirements
* Knowledge of how to add support for a new mainboard in coreboot

### Mentors
* christian.walter@9elements.com
* TBD

## Payloads

The current documentation of the payloads is not very effective. There should be
more detailed documentation on the payloads that can be selected via the make
menuconfig within coreboot. Also the use-cases should be described in more
detail: When to use which payload? What are the benefits of using payload X over
Y in a specific use-case ?

In addition it should be made clear how additional functionality e.g. extend
LinuxBoot with more commands, can be achieved.

### Requirements
* Basic knowledge of the supported payloads like SeaBIOS, TinanoCore, LinuxBoot,
  GRUB, Linux, ...


### Mentors
* christian.walter@9elements.com
* TBD


## coreboot Util Documentation

coreboot inherits a variaty of utilities. The current documentation only
provides a "one-liner" as an explanation. The list of util should be updated
with a more detailed explanation where possible. Also more "in-depths"
explanations should be added with examples if possible.

### Requirements
* coreboot utilities

### Mentors
* christian.walter@9elements.com
* TBD


## CBMEM Developer Guide

CBMEM is the API that provides memory buffers for the use at OS runtime. It's a
core component and thus should be documented. Dos, don'ts and pitfalls when
using CBMEM. This "in-depth" guide is clearly for developers.

### Requirements
* Deep understanding of coreboot's internals

### Mentors
* TBD
* TBD


## CBFS Developer Guide

CBFS is the in-flash filesystem that is used by coreboot. It's a core component
and thus should be documented. Update the existing CBFS.txt that still shows
version 1 of the implementation. A [first approach](https://review.coreboot.org/c/coreboot/+/33663/2)
has been made here.
This "in-depth" guide is clearly for developers.

### Requirements
* Deep understanding of coreboot's internals

### Mentors
* TBD
* TBD


## Region API Developer Guide

The region API is used by coreboot when dealing with memory mapped objects that
can be split into chunks. It's a core component and thus should be documented.
This "in-depth" guide is clearly for developers.

### Requirements
* Deep understanding of coreboot's internals

### Mentors
* TBD
* TBD

