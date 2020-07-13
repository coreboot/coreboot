# Language style

Following our [Code of Conduct](code_of_conduct.md) the project aims to
be a space where people are considerate in natural language communication:

There are terms in computing that were probably considered benign when
introduced but are uncomfortable to some. The project aims to de-emphasize
such terms in favor of alternatives that are at least as expressive -
but often manage to be even more descriptive.

## Political Correctness

A common thread in discussions was that the project merely follows some
fad, or that this is a "political correctness" measure, designed to please
one particular "team". While the project doesn't exist in a vacuum and
so there are outside influences on project members, the proposal wasn't
made with the purpose of demonstrating allegiance to any given cause -
except one:

There are people who feel uncomfortable with some terms being used,
_especially_ when that use takes them out of their grave context
(e.g. slave when discussing slavery) and applies them to a rather benign
topic (e.g. coordination of multiple technical systems), taking away
the gravity of the term.

That gets especially jarring when people aren't exposed to such terms
in abstract sociological discussions but when they stand for real issues
they encountered.

When having to choose between using a well-established term that
affects people negatively who could otherwise contribute more happily
and undisturbed or an alternative just-as-good term that doesn't, the
decision should be simple.

## Token gesture

The other major point of contention is that such decisions are a token
gesture that doesn't change anything. It's true: No slave is freed
because coreboot rejects the use of the word.

coreboot is ambitious enough as-is, in that the project offers
an alternative approach to firmware, sometimes against the vested
interests (and deep pockets) of the leaders of a multi-billion dollar
industry. Changing the preferred vocabulary isn't another attempt at
changing the world, it's one thing we do to try to make coreboot (and
coreboot only) a comfortable environment for everybody.

## For everybody

For everybody, but with a qualifier: We have certain community etiquette,
and we define some behavior we don't accept in our community, both
detailed in the Code of Conduct.

Other than that, we're trying to accommodate people: The CoC lays out
that language should be interpreted as friendly by default, and to be
graceful in light of accidents. This also applies to the use of terms
that the project tries to avoid: The consequence of the use of such
terms (unless obviously employed to provoke a reaction - in that case,
please contact the arbitration team as outlined in the Code of Conduct)
should be a friendly reminder. The project is slow to sanction and that
won't change just because the wrong kind of words is used.

## Interfacing with the world

The project doesn't exist in a vacuum, and that also applies to the choice
of words made by other initiatives in low-level technology. When JEDEC
calls the participants of a SPI transaction "master" and "slave", there's
little we can do about that. We _could_ decide to use different terms,
but that wouldn't make things easier but harder, because such a deliberate
departure means that the original terms (and their original use) gain
lots of visibility every time (so there's no practical advantage) while
adding confusion, and therefore even more attention, to that situation.

Sometimes there are abbreviations that can be used as substitutes,
and in that case the recommendation is to do that.

As terms that we found to be best avoided are replaced in such
initiatives, we can follow up. Members of the community with leverage
in such organizations are encouraged to raise the concern there.

## Dealing with uses

There are existing uses in our documentation and code. When we decide to
retire a term that doesn't mean that everybody is supposed to stop doing
whatever they're doing and spend their time on purging terms. Instead,
ongoing development should look for alternatives (and so this could come
up in review).

People can go through existing code and docs and sort out older instances,
and while that's encouraged it's no "stop the world" event. Changes
in flight in review may still be merged with such terms intact, but if
there's more work required for other reasons, we'd encourage moving away
from such terms.

This document has a section on retired terms, presenting the rationale
as well as alternative terms that could be used instead. The main goal is
to be expressive: There's no point in just picking any alternative term,
choose something that explains the purpose well.

As mentioned, missteps will happen. Point them out, but assume no ill
intent for as long as you can manage.

## Discussing words to remove from active use

There ought to be some process when terminology is brought up as a
negative to avoid. Do not to tell people that "they're feeling wrong"
when they have a negative reaction to certain terms, but also try to
avoid being offended for the sake of others.

When bringing up a term, on the project's mailing list or, if you don't
feel safe doing that, by contacting the arbitration team, explain what's
wrong with the term and offer alternatives for uses within coreboot.

With a term under discussion, see if there's particular value for us to
continue using the term (maybe in limited situations, like continuing
to use "slave" in SPI related code).

Once the arbitration team considers the topic discussed completely and
found a consensus, it will present a decision in a leadership meeting. It
should explain why a term should or should not be used and in the latter
case offer alternatives. These decisions shall then be added to this
document.

## Retired terminology

### slave

Replacing this term for something else had the highest approval rating
in early discussions, so it seems pretty universally considered a bad
choice and therefore should be avoided where possible.

An exception is made where it's a term used in current standards and data
sheets: Trying to "hide" the term in such cases only puts a spotlight
on it every time code and data sheet are compared.

Alternatives: subordinate, secondary, follower
