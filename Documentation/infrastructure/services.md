# Accounts on coreboot.org

There are a number of places where you can benefit from creating an account
in our community. Since there is no single sign-on system in place (at this
time), they come with their own setup routines.

## Gerrit code review
We exchange and review patches to the code using our [Gerrit code review
system](https://review.coreboot.org).

It allows logging in with a Google or GitHub account using OAuth2 as well
as with any OpenID provider that you may already use.

On the [settings screen](https://review.coreboot.org/settings) you can register
all your email addresses you intend to use in the context of coreboot
development so that commits with your email address in them are associated with
you properly.

Below is a list of its SSH host keys and fingerprints.
```Bash
[review.coreboot.org]:29418 ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEAvNDn8qGHlWM/5ndFltStlg3QTc8xvGOgyjxxZByhMZx8LVE4cfgF38WP3euq0avyFy7gAJNghHorXpYKoOzuQPn2WNi5QhyGsUhg7ZJz9hC7Z2gqxxsZF3E7rku4Uj9sN7hWx9fBngxD4z2tP4y/18FTT5XTMcC3Q2sBCOLM0XVAO5R/nb2GO3d27avy+sanKAFEwJHnZ996IoTlU8JJFyi1Y6g30dC2K75oFgCtzntxf++wvrkkKPa+CFQub8fp20shat9WwX9kXjpRjt/Yv9LgqFCaI5ztJvWXicAmbgghGVzbzz4GoSjjF9cxxJF//KTmNb4iGQqmP3Olm27xuw==

[review.coreboot.org]:29418 ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBBzlwf/bFejt4EEz1QmbNOfK/HN1NtdcefrRs5Gs42uGnIvjxsff+vEF3//jCTvFPadoy3DrPsbQB3ioQAcYppk=

[review.coreboot.org]:29418 ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIOC3Z32gc+1rJXhKX+SW0vESlXR/h/mhcxd+62B1PWC2
```

```Bash
2048 SHA256:WW5prF7YE3MTnkRIxLklr9Gxddj9s5BZKUqWJF5dnTg review.coreboot.org:29418 (RSA)
256 SHA256:IuLv/DgrBtVn36eMP1zFD0ISAl3IxIoCeiRms6UDhZc review.coreboot.org:29418 (ECDSA)
256 SHA256:QFZieVHy8dCRl9tDib6qiwELnfa7SVU4ZWJ5VrXoC8k review.coreboot.org:29418 (ED25519)
```

### https push access
When using the https URLs to git repositories, you can push with the "HTTP
Credentials" you can have Gerrit generate for you on that page. By default,
git uses `$HOME/.netrc` for http authentication data, so add a line there
stating:

    machine review.coreboot.org login $your-user-name password $your-password

### Gerrit user avatar
To setup an avatar to show in Gerrit, clone the avatars repository at
https://review.coreboot.org/gerrit-avatars.git and add a file named
$your-user-ID.jpg (the user ID is a number shown on the [settings screen](https://review.coreboot.org/settings)).
The image must be provided in JPEG format, must be square and have at most 50000
bytes.

After you push for review, the system will automatically verify your change
and, if adhering to these constraints, approve it. You can then immediately
submit it.

## Issue tracker
We have an [issue tracker](https://ticket.coreboot.org) that is used for
coreboot and related code, such as libpayload, as well as for the project's
infrastructure.

It can be helpful to refer to issues we track there in commit messages:

    Fixes: https://ticket.coreboot.org/issues/$id
