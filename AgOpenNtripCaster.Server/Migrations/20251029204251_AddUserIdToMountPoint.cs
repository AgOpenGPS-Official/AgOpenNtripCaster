using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AgOpenNtripCaster.Server.Migrations
{
    /// <inheritdoc />
    public partial class AddUserIdToMountPoint : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "OwnerId",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<string>(
                name: "UserId",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.CreateIndex(
                name: "IX_MountPoints_OwnerId",
                table: "MountPoints",
                column: "OwnerId");

            migrationBuilder.AddForeignKey(
                name: "FK_MountPoints_AspNetUsers_OwnerId",
                table: "MountPoints",
                column: "OwnerId",
                principalTable: "AspNetUsers",
                principalColumn: "Id");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropForeignKey(
                name: "FK_MountPoints_AspNetUsers_OwnerId",
                table: "MountPoints");

            migrationBuilder.DropIndex(
                name: "IX_MountPoints_OwnerId",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "OwnerId",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "UserId",
                table: "MountPoints");
        }
    }
}
