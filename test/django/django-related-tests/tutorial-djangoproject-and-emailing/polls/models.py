from django.db import models
# additional tutorial items
import datetime
from django.utils import timezone

# Create your models here.

# following are from tutorial
class Poll(models.Model):
    question = models.CharField(max_length=200)
    pub_date = models.DateTimeField('date published')

    def was_published_recently(self):
        return self.pub_date >= timezone.noew() - datetime.timedelta(days=1)

# for interactive prompt $ Polls.objects.all()
# to display all the polls in the database
    def __unicode__(self):
        return self.question

class Choice(models.Model):
    poll = models.ForeignKey(Poll)
    choice_text = models.CharField(max_length=200)
    votes = models.IntegerField(default=0)

    def __unicode__(self):
        return self.choice_text

