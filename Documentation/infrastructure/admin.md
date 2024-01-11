# Operating our services

## Mailing list moderation

Our [mailing lists] experience the same barrage of spam mails than any
other email address. We do have a spam filter in front of it, and
since the lists require registration, spam ends up in the moderation
queue. But not only spam ends up there, sometimes users send inquiries
without registering first. It's a custom of the project to let these
through, so that such emails can be discussed. This requires manual
intervention.

This section describes the tasks related to mailing list management.

### Registration

To participate in mailing list moderation, you need to become a list
moderator or owner. This is up for the existing owners to handle and
if you want to contribute in that area, it might be best to bring it
up at the leadership meeting.

After gaining leadership approval, list admins can add you to the
appropriate group in the [mailing list backend] by selecting the list,
then User / group-name, and add your email address there.

### Regular tasks

Most of our lists are auto-subscribing, so users can register
themselves and finish the process by responding to the double-opt-in
email. Some lists are manually managed though. The [mailing list
backend] shows the number of open subscription requests for these
lists on the mailing list's main page.

It also provides a list of held messages, where they can be accepted,
rejected or dropped. Spam should be dropped, that's clear. Emails with
huge attachments (e.g. screenshots) should be rejected, which gives
you an opportunity to explain the reason (in case of large
attachments, something like "Please re-send without attachments, offer
the files through some other mechanism please: Our emails are
distributed to hundreds of readers, and sending the files to everybody
is inconsiderate of traffic and storage constraints.")

Legit emails (often simple requests of the form "is this or that
supported") can be accepted, which means they'll be sent out.

If you notice recurring spam sources (e.g. marketers) you can put them
on the [global ban list] to filter them out across all lists. It takes
entries in regular expression format.

[mailing lists]: https://mail.coreboot.org/hyperkitty/
[mailing list backend]: https://mail.coreboot.org/postorius/
[global ban list]: https://mail.coreboot.org/postorius/bans/
