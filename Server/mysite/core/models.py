from __future__ import unicode_literals

from django.db import models

# Create your models here.
class TableNat(models.Model):
    id = models.AutoField(primary_key=True)

    ip = models.TextField()
    port = models.IntegerField()

    def __str__(self):
        return self.ip