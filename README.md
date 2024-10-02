A simple program that makes your repo submodules hashes update in semi-auto mode.
PRs are created automatically, reviewers are added automatically, but you still need to merge manually.

Build steps

mkdir build
cd build
cmake ..
make

Binaries are put in Binaries folder as well as template config file and template pull-request description.
You just need to change some nodes in config file according to your setup

api_url - API url for you gitlab instance. Leave default for GitHub

user_token_env - environment variable name that will be read and dereferenced for you git token string
you have to have this variable defined in your env. e.g. add it to like this
echo "export YOUR_ENV_VAR=ghp_12345678asdfghjk" >> /home/$USER/.bashrc file

add as many reviewers as you need similar to an example under <reviewers> node

Each repo/submodule has a set of nodes
url - obviously your repo URL
branch - a branch name to update
update_branch - name for temporary update branch that you create and then delete
path - submodule path relative to parent repo root folder. don't set for top-level repo

Next few nodes are setting your data that will replace corresponding placeholders in pull_request.md template
update_pr_title
DESCRIPTION_PLACEHOLDER
PLATFORMS_PLACEHOLDER
JIRANUM_PLACEHOLDER
