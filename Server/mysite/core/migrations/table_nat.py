from __future__ import unicode_literals

from django.db import migrations, models


class Migration(migrations.Migration):


    operations = [
        migrations.CreateModel(
            name='TableNat',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('ip', models.CharField(max_length=20)),
                ('port', models.IntegerField(max_length=20)),
            ],
        ),
]