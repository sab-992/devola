import { Component } from '@angular/core';
import { ThemeComponent } from "@components/theme/theme";
import { RouterLink } from '@angular/router';

@Component({
    selector: 'app-not-found',
    imports: [ThemeComponent, RouterLink],
    templateUrl: './not-found.html',
    styleUrl: './not-found.scss',
})
export class NotFoundPage { }
